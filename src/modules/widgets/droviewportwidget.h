#ifndef DROVIEWPORTWIDGET_H
#define DROVIEWPORTWIDGET_H

#include <QObject>
#include <QWidget>
#include <drcharactermovie.h>
#include <dreffectmovie.h>
#include <drscenemovie.h>
#include <drsplashmovie.h>
#include <drgraphicscene.h>
#include <drshoutmovie.h>
#include "modules/managers/scenario_manager.h"
#include "keyframe_player.h"
#include "typewritertextedit.h"

#include <mk2/graphicsvideoscreen.h>

#include <aolabel.h>

#include <modules/background/background_data.h>

class DROViewportWidget : public DRGraphicsView
{
  Q_OBJECT
public:
  explicit DROViewportWidget(QWidget *parent = nullptr, double m_Scale = 1);
  void ConstructViewport(ThemeSceneType t_scene);
  void ProcessIncomingMessage(ICMessageData *t_IncomingMessage);

  void TransitionRender();
  void TransitionAnimate();

  //
  void ConstructUserInterface();
  void ConstructText();
  void PlayShoutAnimation(QString t_name);
  void ToggleChatbox(bool t_state);


  void SetViewportWeather(QString t_weather);

  //
  void PlaySplashAnimation(QString t_name);

  //Visual Updates
  void SetBackground(QString t_background = "Blackout_HD");
  void UpdateBackground(QString t_position = "wit");

  bool isPlayingAnimation()
  {
    return false;
  }

  AOApplication *m_AOApp = nullptr;

signals:
  void VideoDone();
  void PreanimDone();

public slots:
  void OnObjectionDone();
  void OnVideoDone();
  void OnPreanimDone();

private:

  DRGraphicsView *m_UserInterface = nullptr;
  KeyframePlayer *m_ShoutsPlayer = nullptr;

  //User Interface
  QMap<QString, DRSceneMovie*> m_UserInterfaceObjects = {};

  TypewriterTextEdit *m_TextMessage = nullptr;
  DRTextEdit *m_TextShowname = nullptr;


  //Data
  ICMessageData *m_IncomingMessage = nullptr;

  //Graphics Items
  DRSceneMovie *m_VpBackground = nullptr;
  DRCharacterMovie *m_VpPlayer = nullptr;
  DRSceneMovie *m_VpWeather = nullptr;


  //Animations
  DREffectMovie *m_VpEffect = nullptr;
  DRSplashMovie *m_VpWtce = nullptr;
  DRShoutMovie *m_VpShouts = nullptr;
  DRVideoScreen *m_VpVideo = nullptr;

  //Transition
  AOLabel *m_WidgetTransition = nullptr;
  int m_FadeDuration = 500;
  BackgroundData *p_BackgroundCurrent = nullptr;
  QString m_CurrentBackground = "Blackout_HD";

  //Resolution
  int m_ViewportWidth = 960;
  int m_ViewportHeight = 544;
  double m_ViewportScale = 1;


};

#endif // DROVIEWPORTWIDGET_H
