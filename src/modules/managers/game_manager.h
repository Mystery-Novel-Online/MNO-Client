#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QWidget>
#include "datatypes.h"

#include <modules/theme/graphicobjectanimator.h>
#include "modules/widgets/typewritertextedit.h"
#include "qthread.h"
#include <drserversocket.h>
#include <modules/networking/dro_packet.h>
#include <modules/networking/network_handler.h>
#include <modules/networking/legacy_network_handler.h>
#include <modules/networking/neo_network_handler.h>

class GameManager : public QObject
{
  Q_OBJECT
public:
  GameManager(const GameManager&) = delete;

  static GameManager& get()
  {
    return s_Instance;
  }

  void StartGameLoop();
  void StopGameLoop();
  void RestartGameLoop();

  NetworkHandler *GetNetworkHandlerCurrent()
  {
    return m_CurrentNetworkHandler;
  };

  bool IsGameLoopRunning()
  {
    return m_IsUpdateRunning;
  }

  void SetPlayerAnimation(GraphicObjectAnimator * t_animation);
  void SetTypeWriter(TypewriterTextEdit * t_writer);
  void SetAnimationGroup(AnimTypes t_type, QVector<GraphicObjectAnimator *> t_animations);

  void UpdateAnimationLoop(AnimTypes t_type);
  void RenderAnimationLoop(AnimTypes t_type);

  //Data Gathering
  QString getShoutName(int t_id) { for(GameShoutData r_ShoutData : m_GameShouts) { if(t_id == r_ShoutData.mLegacyId) return r_ShoutData.mName; } return ""; }
  GameEffectData getEffect(QString t_name) { for(GameEffectData rEffectData : m_GameEffects) { if(t_name == rEffectData.mName) return rEffectData; } return GameEffectData("<NONE>"); }
  GameEffectData getEffect(int t_id) { for(GameEffectData rEffectData : m_GameEffects) { if(t_id == rEffectData.mLegacyId) return rEffectData; } return GameEffectData("<NONE>"); }

  //Setup
  void setupGame();


  //Server Stuff
  void SetupNetworkHandler();
  bool ProcessIncomingPacket(QString t_operation, QStringList t_parameters);
  void setServerFunctions(QStringList tFunctionList);
  bool usesServerFunction(QString tFunctionName);


  int getUptime();


public slots:
  void RunGameLoop();

signals:
  void FrameComplete();
  void JudgeComplete();
  void ShoutComplete();


private:

private:
  GameManager() {}
  static GameManager s_Instance;

  //Networking
  NetworkHandler *m_CurrentNetworkHandler = nullptr;

  //Game Loop Variables
  int m_FramesPerSecond = 60;
  bool m_IsUpdateRunning = false;

  //Game Timings
  QTimer m_FrameTimer;
  int m_GameUptime = 0;

  //Widgets
  TypewriterTextEdit *m_WidgetTypeWriter = nullptr;

  //Animations
  GraphicObjectAnimator *m_PlayerAnimation = nullptr;
  QMap<AnimTypes, QVector<GraphicObjectAnimator *>> m_GraphicObjectAnimations = {};

  //Data
  QVector<GameEffectData> m_GameEffects = {};
  QVector<GameShoutData> m_GameShouts = {};
  QStringList m_ServerFeatures = {};

  void setupGameEffects();
  void setupGameShouts();
};

#endif // GAMEMANAGER_H
