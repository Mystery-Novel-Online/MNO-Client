#include "pch.h"
#include "legacy_viewport.h"
#include "drcharactermovie.h"
#include "engine/network/metadata/message_metadata.h"
#include "drscenemovie.h"
#include "theme.h"

#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_characters.h"

#include "engine/param/background/legacy_background_reader.h"
#include "engine/param/background/background_reader.h"
#include "engine/param/actor/actor_loader.h"

#include "modules/theme/thememanager.h"

#include "engine/param/actor_repository.h"
#include "engine/system/runtime_loop.h"
#include "engine/system/audio.h"
#include "engine/system/theme.h"


using namespace engine::network::metadata;

LegacyViewport::LegacyViewport(QWidget *parent) : RPViewport(parent)
{
  m_graphicsView = new DRGraphicsView(parent);
  engine::system::theme::applyDimensions(m_graphicsView, "viewport", SceneType_Replay);
  RuntimeLoop::assignViewport(this);
}

void LegacyViewport::update()
{
  m_graphicsView->scene()->update();
}

void LegacyViewport::constructViewport()
{
  AOApplication *aoApp = AOApplication::getInstance();
  m_backgroundSprite = new DRSceneMovie(aoApp);
  m_characterSprite = new DRCharacterMovie(aoApp);
  m_pairSprite = new DRCharacterMovie(aoApp);

  m_videoScreen = new DRVideoScreen(aoApp);
  m_shoutMovie = new DRShoutMovie(aoApp);
  m_effectMovie = new DREffectMovie(aoApp);

  m_graphicsView->scene()->addItem(m_backgroundSprite);
  m_graphicsView->scene()->addItem(m_pairSprite);
  m_graphicsView->scene()->addItem(m_characterSprite);

  m_graphicsView->scene()->addItem(m_effectMovie);
  m_graphicsView->scene()->addItem(m_shoutMovie);
  m_graphicsView->scene()->addItem(m_videoScreen);



  m_backgroundSprite->setZValue(ViewportLayers_BG);
  m_pairSprite->setZValue(ViewportLayers_PlayerBack);
  m_characterSprite->setZValue(ViewportLayers_PlayerFront);

  //ui_vp_desk->setZValue(ViewportLayers_Desk);
  //ui_vp_weather->setZValue(ViewportLayers_Weather);

  m_effectMovie->setZValue(ViewportLayers_Effect);

  //ui_vp_wtce->setZValue(ViewportLayers_WTCE);

  m_shoutMovie->setZValue(ViewportLayers_Objection);
  m_videoScreen->setZValue(ViewportLayers_Video);

  m_backgroundSprite->start();
  m_characterSprite->start();
  m_pairSprite->start();

  m_transitionWidget = new RPLabel(m_graphicsView, aoApp);
  m_transitionWidget->resize(m_graphicsView->width(), m_graphicsView->height());

  connect(m_shoutMovie, SIGNAL(done()), this, SLOT(onObjectionDone()));
  connect(m_videoScreen, SIGNAL(finished()), this, SLOT(onVideoDone()));
  connect(m_characterSprite, SIGNAL(done()), this, SLOT(onPreanimDone()));

  constructInterface();
}

void LegacyViewport::loadCurrentMessage()
{
  MessageMetadata &message = message::recentMessage();
  m_currentActor = engine::actor::repository::retrieve(message.characterFolder);
  m_pairActor = engine::actor::repository::retrieve(message.pairData.characterFolder);
  if(message.characterPre.trimmed().isEmpty()) message.characterPre = "-";
  m_message->setInput("");
  toggleChatbox(false);

  if(message.userShowname.isEmpty())
  {
    m_showname->setText(m_currentActor->GetShowname());
  }
  else
  {
    m_showname->setText(message.userShowname);
  }
  crossfadeRender();

  refreshBackground(message.backgroundPosition);
  if (!m_videoScreen->isVisible()) m_videoScreen->show();

  crossfadeStart();

  m_shoutMovie->stop();
  m_effectMovie->stop();
  m_pairSprite->hide();

  {
    if(!message.pairData.characterFolder.trimmed().isEmpty())
    {
      m_pairSprite->set_play_once(false);
      m_pairSprite->set_file_name(fs::characters::getSpritePathIdle(message.pairData.characterFolder, message.pairData.characterEmote));

      m_pairSprite->setHorizontalOffset(message::pair::horizontalOffset());
      m_pairSprite->setVerticalOffset(message::pair::verticalOffset());

      mk2::SpritePlayer::ScalingMode targetScaling = m_pairActor->GetScalingMode();
      m_pairSprite->start(targetScaling, (double)message.pairData.offsetScale / 1000.0f);
      m_pairSprite->processOverlays(message.pairData.characterLayers, message.pairData.characterFolder, message.pairData.characterEmote, message.pairData.characterOutfit);
      m_pairSprite->setCharacterAnimation(message.pairData.characterSequence, message.pairData.characterFolder, true);
      m_pairSprite->show();
    }
  }

  m_videoScreen->play_character_video(message.characterFolder, message.characterVideo);
}

void LegacyViewport::loadBackground(QString background)
{
  if(background == m_backgroundName) return;
  m_backgroundName = background;

  AOApplication *aoApp = AOApplication::getInstance();
  const QString jsonPath = aoApp->find_asset_path(aoApp->get_background_path(m_backgroundName) + "/" + "background.json");

  if(FS::Checks::FileExists(jsonPath))
    m_backgroundData = new BackgroundReader();
  else
    m_backgroundData = new LegacyBackgroundReader();

  m_backgroundData->loadBackground(m_backgroundName.toStdString());
  refreshBackground("wit");
}

void LegacyViewport::refreshBackground(QString position)
{
  if(m_backgroundData == nullptr) return;
  std::string l_filename = m_backgroundData->backgroundFilename(position.toStdString());
  m_backgroundSprite->set_file_name(AOApplication::getInstance()->get_background_sprite_path(m_backgroundName, QString::fromStdString(l_filename)));
  m_backgroundSprite->start();
}

void LegacyViewport::toggleChatbox(bool state)
{
  if(m_userInterfaceObjects.contains("chatbox"))
  {
    if(state)
    {
      m_userInterfaceObjects["chatbox"]->show();
      m_showname->show();
    }
    else
    {
      m_userInterfaceObjects["chatbox"]->hide();
      m_showname->hide();
    }
  }
}

void LegacyViewport::crossfadeStart()
{
  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect();
  m_transitionWidget->setGraphicsEffect(eff);

  QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
  a->setDuration(m_fadeDuration);
  a->setStartValue(1);
  a->setEndValue(0);
  a->setEasingCurve(QEasingCurve::OutBack);
  a->start(QPropertyAnimation::DeleteWhenStopped);
}

void LegacyViewport::crossfadeRender()
{
  QImage image(m_graphicsView->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
  image.fill(Qt::transparent);

  QPainter painter(&image);
  m_graphicsView->scene()->render(&painter);
  m_transitionWidget->setPixmap(QPixmap::fromImage(image));
}

void LegacyViewport::onTypingDone()
{
  emit textDone();
}

void LegacyViewport::onVideoDone()
{
  emit videoDone();
  if (m_videoScreen->isVisible()) m_videoScreen->hide();

  MessageMetadata &message = message::recentMessage();
  m_characterSprite->setHorizontalOffset(message.offsetHorizontal);
  m_characterSprite->setVerticalOffset(message.offsetVertical);
  m_characterSprite->processOverlays(message.characterLayers, message.characterFolder, message.characterEmote, message.characterOutfit);

  if (message.characterShout.isEmpty())
  {
    onObjectionDone();
    return;
  }
  message.modifiers.PreAnimation = true;
  m_shoutMovie->set_play_once(true);
  m_shoutMovie->set_file_name(AOApplication::getInstance()->get_shout_sprite_path(message.characterFolder, message.characterShout));
  m_shoutMovie->start();
}

void LegacyViewport::onObjectionDone()
{
  MessageMetadata &message = message::recentMessage();
  if(!message.characterPre.isEmpty())
  {
    m_characterSprite->set_play_once(true);
    m_characterSprite->set_file_name(fs::characters::getSpritePathPre(message.characterFolder, message.characterPre));
    mk2::SpritePlayer::ScalingMode targetScaling = m_currentActor->GetScalingMode();
    m_characterSprite->start(targetScaling, (double)message.offsetScale / 1000.0f);
  }

  QString effectName = message.effect.name;
  if(effectName == "<NONE>" || effectName.isEmpty()) m_effectMovie->hide();
  else
  {
    m_effectMovie->show();
    m_effectMovie->set_play_once(!message.effect.loops);
    m_effectMovie->set_file_name(AOApplication::getInstance()->get_effect_anim_path(effectName));
    m_effectMovie->start();

    QString l_overlay_sfx = AOApplication::getInstance()->get_sfx(effectName);
    audio::effect::Play(l_overlay_sfx.toStdString());
  }

  audio::effect::PlayCharacter(message.characterFolder.toStdString(), message.sfxName.toStdString());

  m_characterSprite->setCharacterAnimation(message.characterSequence, message.characterFolder);
}

void LegacyViewport::onPreanimDone()
{
  emit preanimDone();
  MessageMetadata &message = message::recentMessage();
  if(!message.textContent.trimmed().isEmpty()) toggleChatbox(true);
  else onTypingDone();
  m_message->setInput(message.textContent);

  if(message.modifiers.Hidden)
  {
    m_characterSprite->hide();
  }
  else
  {
    m_characterSprite->show();
    m_characterSprite->set_play_once(false);
    m_characterSprite->set_file_name(fs::characters::getSpritePathIdle(message.characterFolder, message.characterEmote));
    mk2::SpritePlayer::ScalingMode targetScaling = m_currentActor->GetScalingMode();
    m_characterSprite->start(targetScaling, (double)message.offsetScale / 1000.0f);
  }
}

void LegacyViewport::constructInterface()
{
  AOApplication *aoApp = AOApplication::getInstance();
  m_userInterface = new DRGraphicsView(m_graphicsView);
  m_userInterface->resize(m_graphicsView->width(), m_graphicsView->height());

  m_userInterface->setStyleSheet("background: transparent;");
  m_userInterface->setBackgroundBrush(Qt::transparent);

  QStringList viewporWidgets = ThemeManager::get().mCurrentThemeReader.GetSceneWidgetNames(SceneType_Viewport);
  QStringList blacklistedWidgets = { "showname" };
  std::reverse(viewporWidgets.begin(), viewporWidgets.end());

  for(QString widgetName : viewporWidgets)
  {
    if(blacklistedWidgets.contains(widgetName)) continue;

    DRSceneMovie* interfaceOverlay = new DRSceneMovie(aoApp);
    m_userInterface->scene()->addItem(interfaceOverlay);
    interfaceOverlay->show();
    interfaceOverlay->set_file_name(aoApp->find_theme_asset_path("vp_" + widgetName + ".png"));
    interfaceOverlay->start();
    m_userInterfaceObjects[widgetName] = interfaceOverlay;
  }

  m_message = new RPTypewriter(m_graphicsView);
  engine::system::theme::applyDimensions(m_message, "message", SceneType_Viewport);

  m_message->setFrameStyle(QFrame::NoFrame);
  m_message->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_message->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_message->setReadOnly(true);
  set_drtextedit_font(m_message, "message", VIEWPORT_FONTS_INI, aoApp);

  m_message->raise();
  connect(m_message, &RPTypewriter::typingDone, this, &LegacyViewport::onTypingDone);

  m_showname = new RPTextEdit("showname");
  engine::system::theme::applyDimensions(m_showname, "showname", SceneType_Viewport);
  m_showname->setFrameStyle(QFrame::NoFrame);
  m_showname->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_showname->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_showname->setReadOnly(true);
  set_drtextedit_font(m_showname, "showname", VIEWPORT_FONTS_INI, AOApplication::getInstance());


  QGraphicsProxyWidget* pProxy = m_userInterface->scene()->addWidget(m_showname);

  WidgetThemeData * l_shownameData = ThemeManager::get().mCurrentThemeReader.GetWidgetData(SceneType_Viewport, "showname");


  if(l_shownameData != nullptr)
  {
    pProxy->setRotation(l_shownameData->Rotation);
    pProxy->setPos(l_shownameData->Transform.x, l_shownameData->Transform.y);
  }
  pProxy->show();
}
