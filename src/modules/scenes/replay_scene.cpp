#include "replay_scene.h"
#include "commondefs.h"
#include "modules/theme/thememanager.h"
#include "modules/managers/scene_manager.h"
#include "qevent.h"
#include "modules/managers/game_manager.h"

#include <modules/managers/audio_manager.h>
#include <modules/managers/replay_manager.h>

ReplayScene::ReplayScene(AOApplication *p_ao_app, QWidget *parent) : QWidget(parent)
{
  pAOApp = p_ao_app;
  constructWidgets();
  setWindowTitle("Replay");
  setFocusPolicy(Qt::StrongFocus);
  setAttribute(Qt::WA_DeleteOnClose, true );
}

ReplayScene::~ReplayScene()
{
  GameManager::get().SetTypeWriter(nullptr);
}

void ReplayScene::playSong(QString t_song)
{
  AudioManager::get().BGMPlay(t_song);
}

void ReplayScene::setMsgOperation(QMap<QString, QString> t_vars)
{
  mMsgVariables = t_vars;

  ICMessageData *m_MessageData = new ICMessageData({}, false);
  m_MessageData->m_ShowName = mMsgVariables["showname"];
  m_MessageData->m_CharacterFolder = mMsgVariables["char"];

  m_MessageData->m_CharacterOutfit = mMsgVariables["outfit"];
  m_MessageData->m_SFXName = mMsgVariables["sound"];
  m_MessageData->m_ShoutName = mMsgVariables["shout"];
  m_MessageData->m_CharacterEmotion = mMsgVariables["emote"];
  m_MessageData->m_PreAnimation = mMsgVariables["pre"];
  m_MessageData->m_VideoName = mMsgVariables["video"];
  m_MessageData->m_AreaPosition = mMsgVariables["pos"];
  m_MessageData->m_HideCharacter = mMsgVariables["hide"] == "1";
  m_MessageData->m_IsFlipped = mMsgVariables["flip"] == "1";
  m_MessageData->m_EffectData = GameManager::get().getEffect(mMsgVariables["effect"]);
  m_MessageData->m_MessageContents = mMsgVariables["msg"];

  m_Viewport->ToggleChatbox(false);

  m_Viewport->ProcessIncomingMessage(m_MessageData);

}

void ReplayScene::setBackground(QString t_name)
{
  SceneManager::get().execLoadPlayerBackground(t_name);
  m_Viewport->UpdateBackground("wit");
}

void ReplayScene::setWeather(QString t_weather)
{
  m_Viewport->SetViewportWeather(t_weather);
}

void ReplayScene::playWTCE(QString t_name)
{
  m_Viewport->PlaySplashAnimation(t_name);
}

void ReplayScene::progressSlider(int t_progress)
{
  m_PlaybackScrubber->setSliderPosition(t_progress);
}

void ReplayScene::SetupReplayMetadata(int t_operationsCount)
{
  m_PlaybackScrubber->setMinimum(0);
  m_PlaybackScrubber->setMaximum(t_operationsCount - 1);
}


void ReplayScene::videoDone()
{

}

void ReplayScene::preanim_done()
{
  if(!mMsgVariables["msg"].trimmed().isEmpty()) m_Viewport->ToggleChatbox(true);
}

void ReplayScene::OnScrubberSliderReleased()
{
  ReplayManager::get().PlaybackProgressSlider(m_PlaybackScrubber->value());
}

void ReplayScene::constructWidgets()
{
  ThemeManager::get().RegisterWidgetGenericBulk({});

  m_Viewport = new ViewportScene(this);
  ThemeManager::get().AutoAdjustWidgetDimensions(m_Viewport, "viewport", SceneTypeReplays);
  ThemeManager::get().RegisterWidgetGeneric("viewport", m_Viewport);

  m_Viewport->ConstructViewport(SceneTypeReplays);
  SceneManager::get().CreateTransition(this, AOApplication::getInstance(), m_Viewport);

  //Replay Controls
  m_PlaybackScrubber = new QSlider(Qt::Horizontal, this);
  ThemeManager::get().AutoAdjustWidgetDimensions(m_PlaybackScrubber, "scrubber", SceneTypeReplays);
  ThemeManager::get().RegisterWidgetGeneric("scrubber", m_PlaybackScrubber);


  //Connections
  connect(m_Viewport, SIGNAL(VideoDone()), this, SLOT(videoDone()));
  connect(m_Viewport, SIGNAL(PreanimDone()), this, SLOT(preanim_done()));
  connect(m_PlaybackScrubber, SIGNAL(sliderReleased()), this, SLOT(OnScrubberSliderReleased()));

  //Extra Stuff
         //ThemeManager::get().createButtonWidget("return_to_lobby", this);

}


void ReplayScene::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Space)
  {
    ReplayManager::get().PlaybackProgressManual();
  }
  else
  {
    QWidget::keyPressEvent(event);
  }
}
