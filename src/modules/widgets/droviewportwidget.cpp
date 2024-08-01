#include "droviewportwidget.h"

#include <QGraphicsOpacityEffect>
#include <QGraphicsProxyWidget>
#include <QPropertyAnimation>
#include <drpather.h>
#include <mk2/spritecachingreader.h>
#include <modules/theme/thememanager.h>
#include "aoapplication.h"
#include "commondefs.h"
#include "file_functions.h"
#include <modules/managers/audio_manager.h>
#include <modules/managers/scene_manager.h>
#include <modules/managers/variable_manager.h>

DROViewportWidget::DROViewportWidget(QWidget *parent) : DRGraphicsView{parent}
{
  m_AOApp = AOApplication::getInstance();
}

void DROViewportWidget::ConstructViewport(ThemeSceneType t_scene)
{
  //Create the items that will populate the GraphicsView.
  m_VpBackground = new DRSceneMovie(m_AOApp);
  m_VpPlayer = new DRCharacterMovie(m_AOApp);
  m_VpEffect = new DREffectMovie(m_AOApp);
  m_VpVideo = new DRVideoScreen(m_AOApp);
  m_VpShouts = new DRShoutMovie(m_AOApp);
  m_VpWtce = new DRSplashMovie(m_AOApp);
  m_VpWeather = new DRSceneMovie(m_AOApp);

  //Add items to the scene.
  this->scene()->addItem(m_VpBackground);
  this->scene()->addItem(m_VpPlayer);
  this->scene()->addItem(m_VpEffect);
  this->scene()->addItem(m_VpWeather);
  this->scene()->addItem(m_VpVideo);
  this->scene()->addItem(m_VpShouts);
  this->scene()->addItem(m_VpWtce);

  m_VpBackground->start();
  m_VpPlayer->start();

  m_WidgetTransition = new AOLabel(this, m_AOApp);
  m_WidgetTransition->resize(this->width(), this->height());

  //Connections
  connect(m_VpShouts, SIGNAL(done()), this, SLOT(OnObjectionDone()));
  connect(m_VpVideo, SIGNAL(finished()), this, SLOT(OnVideoDone()));
  connect(m_VpPlayer, SIGNAL(done()), this, SLOT(OnPreanimDone()));

  ConstructUserInterface();
}

void DROViewportWidget::ProcessIncomingMessage(ICMessageData *t_IncomingMessage)
{
  //m_ShoutsPlayer->stopAnimation();
  VariableManager::get().setMessageVariables(t_IncomingMessage);

  if(!t_IncomingMessage->m_CharacterOutfit.trimmed().isEmpty()) t_IncomingMessage->m_CharacterFolder = (t_IncomingMessage->m_CharacterFolder + "/outfits/" +  t_IncomingMessage->m_CharacterOutfit);
  if(t_IncomingMessage->m_PreAnimation.trimmed().isEmpty()) t_IncomingMessage->m_PreAnimation = "-";
  m_TextMessage->setTypewriterTarget("");
  m_TextShowname->setText(t_IncomingMessage->m_ShowName);

  TransitionRender();
  UpdateBackground(t_IncomingMessage->m_AreaPosition);
   m_IncomingMessage = t_IncomingMessage;

  if (!m_VpVideo->isVisible())
  {
    m_VpVideo->show();
  }

  TransitionAnimate();
  m_VpShouts->stop();
  m_VpEffect->stop();
  m_VpVideo->play_character_video(m_IncomingMessage->m_CharacterFolder, m_IncomingMessage->m_VideoName);
}

void DROViewportWidget::TransitionRender()
{
  QImage image(this->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
  image.fill(Qt::transparent);

  QPainter painter(&image);
  this->scene()->render(&painter);
  m_WidgetTransition->setPixmap(QPixmap::fromImage(image));
}

void DROViewportWidget::TransitionAnimate()
{
  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect();
  m_WidgetTransition->setGraphicsEffect(eff);

  QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
  a->setDuration(m_FadeDuration);
  a->setStartValue(1);
  a->setEndValue(0);
  a->setEasingCurve(QEasingCurve::OutBack);
  a->start(QPropertyAnimation::DeleteWhenStopped);
}

void DROViewportWidget::ConstructUserInterface()
{
  m_UserInterface = new DRGraphicsView(this);
  m_UserInterface->resize(960, 544);

  m_UserInterface->setStyleSheet("background: transparent;");
  m_UserInterface->setBackgroundBrush(Qt::transparent);


  QStringList l_widgetNames = ThemeManager::get().mCurrentThemeReader.GetSceneWidgetNames(SceneTypeViewport);

  QStringList l_WidgetBlacklist = { "showname" };

  for(QString r_name : l_widgetNames)
  {
    if(l_WidgetBlacklist.contains(r_name)) continue;

    DRSceneMovie* l_uiObject = new DRSceneMovie(m_AOApp);
    m_UserInterface->scene()->addItem(l_uiObject);
    l_uiObject->show();
    l_uiObject->set_file_name(m_AOApp->find_theme_asset_path("vp_" + r_name + ".png"));
    l_uiObject->start();
    m_UserInterfaceObjects[r_name] = l_uiObject;
  }

  m_UserInterface->show();

  ConstructText();

  m_ShoutsPlayer = new KeyframePlayer(this);
  m_ShoutsPlayer->resize(960, 544);
  connect(m_ShoutsPlayer, SIGNAL(ShoutAnimationFinished()), this, SLOT(OnObjectionDone()));

}

void DROViewportWidget::ConstructText()
{
  m_TextMessage = new TypewriterTextEdit(this);
  ThemeManager::get().AutoAdjustWidgetDimensions(m_TextMessage, "message", SceneTypeViewport);
  m_TextMessage->setFrameStyle(QFrame::NoFrame);
  m_TextMessage->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_TextMessage->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_TextMessage->setReadOnly(true);
  GameManager::get().SetTypeWriter(m_TextMessage);
  set_drtextedit_font(m_TextMessage, "message", VIEWPORT_FONTS_INI, m_AOApp);

  m_TextShowname = new DRTextEdit();
  ThemeManager::get().AutoAdjustWidgetDimensions(m_TextShowname, "showname", SceneTypeViewport);
  m_TextShowname->setFrameStyle(QFrame::NoFrame);
  m_TextShowname->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_TextShowname->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_TextShowname->setReadOnly(true);
  set_drtextedit_font(m_TextShowname, "showname", VIEWPORT_FONTS_INI, m_AOApp);


  QGraphicsProxyWidget* pProxy = m_UserInterface->scene()->addWidget(m_TextShowname);

  WidgetThemeData * l_shownameData = ThemeManager::get().mCurrentThemeReader.GetWidgetData(SceneTypeViewport, "showname");

  if(l_shownameData != nullptr)
  {
    pProxy->setRotation(l_shownameData->Rotation);
    pProxy->setPos(l_shownameData->Transform.x, l_shownameData->Transform.y);
  }
  pProxy->show();
}

void DROViewportWidget::PlayShoutAnimation(QString t_name)
{
  m_ShoutsPlayer->playAnimation(t_name, eAnimationShout);
}

void DROViewportWidget::ToggleChatbox(bool t_state)
{
  if(m_UserInterfaceObjects.contains("chatbox"))
  {
    if(t_state) m_UserInterfaceObjects["chatbox"]->show();
    else m_UserInterfaceObjects["chatbox"]->hide();
  }
}

void DROViewportWidget::SetViewportWeather(QString t_weather)
{
  QString l_WeatherDirPath = DRPather::SearchPathFirst("animations/weather/" + t_weather + "/");

  VariableManager::get().setVariable("weather", t_weather);

  if(!dir_exists(l_WeatherDirPath) || t_weather.trimmed().isEmpty())
  {
    m_VpWeather->stop();
    m_VpWeather->hide();
    AudioManager::get().WeatherPlay("");
    return;
  }



  JSONReader l_WeatherParam = JSONReader();
  l_WeatherParam.ReadFromFile(l_WeatherDirPath + "param.json");

  AudioManager::get().WeatherPlay(l_WeatherParam.getStringValue("sound"));


  const QString l_file_name = l_WeatherDirPath + "overlay.webp";

  auto l_viewer = m_VpWeather->get_reader();
  const QString l_current_file_name = l_viewer->get_file_name();

  if (l_file_name != l_current_file_name)
  {
    m_VpWeather->set_reader(mk2::SpriteReader::ptr(new mk2::SpriteCachingReader));
    m_VpWeather->set_file_name(l_file_name);
  }

  m_VpWeather->start();
  m_VpWeather->show();
}

void DROViewportWidget::PlaySplashAnimation(QString t_name)
{
  if(!m_ShoutsPlayer->playAnimation(t_name, eAnimationGM))
  {
    m_VpWtce->play(t_name);
  }

}

void DROViewportWidget::UpdateBackground(QString t_position)
{
  m_VpBackground->set_file_name(SceneManager::get().getBackgroundPath(t_position));
  m_VpBackground->start();
}

void DROViewportWidget::OnObjectionDone()
{

  if(!m_IncomingMessage->m_PreAnimation.isEmpty())
  {
    m_VpPlayer->set_play_once(true);
    m_VpPlayer->set_file_name(m_AOApp->get_character_sprite_pre_path(m_IncomingMessage->m_CharacterFolder, m_IncomingMessage->m_PreAnimation));
    m_VpPlayer->start();
  }

  QString l_EffectName = m_IncomingMessage->m_EffectData.mName;
  if(l_EffectName == "<NONE>" || l_EffectName.isEmpty())
  {
    m_VpEffect->hide();
  }
  else
  {
    m_VpEffect->show();
    m_VpEffect->set_play_once(!m_IncomingMessage->m_EffectData.mLoops);
    m_VpEffect->set_file_name(m_AOApp->get_effect_anim_path(l_EffectName));
    m_VpEffect->start();

    QString l_overlay_sfx = m_AOApp->get_sfx(l_EffectName);
    AudioManager::get().PlaySFX(l_overlay_sfx);
  }

  AudioManager::get().PlaySFXCharacter(m_IncomingMessage->m_SFXName, m_IncomingMessage->m_CharacterFolder);

}

void DROViewportWidget::OnVideoDone()
{
  emit VideoDone();
  if (m_VpVideo->isVisible()) m_VpVideo->hide();

  if (m_IncomingMessage->m_ShoutName.isEmpty())
  {
    OnObjectionDone();
    return;
  }

  if(!m_ShoutsPlayer->playAnimation(m_IncomingMessage->m_ShoutName, eAnimationShout))
  {
    m_IncomingMessage->m_UsesPreAnimation = true;
    m_VpShouts->set_play_once(true);
    m_VpShouts->set_file_name(m_AOApp->get_shout_sprite_path(m_IncomingMessage->m_CharacterFolder, m_IncomingMessage->m_ShoutName));
    m_VpShouts->start();
  }


}

void DROViewportWidget::OnPreanimDone()
{
  emit PreanimDone();
  m_TextMessage->setTypewriterTarget(m_IncomingMessage->m_MessageContents);

  if(m_IncomingMessage->m_HideCharacter)
  {
    m_VpPlayer->hide();
  }
  else
  {
    m_VpPlayer->show();
    m_VpPlayer->set_play_once(false);
    m_VpPlayer->set_file_name(m_AOApp->get_character_sprite_idle_path(m_IncomingMessage->m_CharacterFolder, m_IncomingMessage->m_CharacterEmotion));
    m_VpPlayer->start();
  }
}
