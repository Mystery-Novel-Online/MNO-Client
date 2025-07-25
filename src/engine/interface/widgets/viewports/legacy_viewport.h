#ifndef LEGACY_VIEWPORT_H
#define LEGACY_VIEWPORT_H
#include "dreffectmovie.h"
#include "engine/interface/widgets/viewports/rp_viewport.h"
#include "drshoutmovie.h"
#include "mk2/graphicsvideoscreen.h"
#include "rolechat/background/IBackgroundData.h"

class RPTypewriter;
class DRSceneMovie;
class DRCharacterMovie;
class ActorData;

class LegacyViewport : public RPViewport
{
  Q_OBJECT
public:
  LegacyViewport(QWidget *parent = nullptr);

public:
  void update() override;
  void constructViewport() override;
  void loadCurrentMessage() override;
  void loadBackground(QString background) override;
  void refreshBackground(QString position) override;
  void toggleChatbox(bool state) override;

  void crossfadeStart();
  void crossfadeRender();

public slots:
  void onTypingDone();
  void onVideoDone();
  void onObjectionDone();
  void onPreanimDone();

private:
  void constructInterface();

  DRGraphicsView *m_graphicsView = nullptr;
  DRGraphicsView *m_userInterface = nullptr;
  QMap<QString, DRSceneMovie*> m_userInterfaceObjects = {};

  RPTypewriter *m_message = nullptr;
  RPTextEdit *m_showname = nullptr;

  DRSceneMovie *m_backgroundSprite = nullptr;
  DRCharacterMovie *m_characterSprite = nullptr;
  DRCharacterMovie *m_pairSprite = nullptr;
  DRSceneMovie *m_weatherSprite = nullptr;


  RPLabel *m_transitionWidget = nullptr;
  int m_fadeDuration = 500;

  //Animations
  DRVideoScreen *m_videoScreen = nullptr;
  DREffectMovie *m_effectMovie = nullptr;
  DRShoutMovie *m_shoutMovie = nullptr;

  //Backgrounds
  rolechat::background::IBackgroundData *m_backgroundData = nullptr;
  QString m_backgroundName = "Blackout_HD";

  rolechat::actor::IActorData *m_currentActor = nullptr;
  rolechat::actor::IActorData *m_pairActor = nullptr;

};

#endif // LEGACY_VIEWPORT_H
