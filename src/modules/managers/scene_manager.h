#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "src/drgraphicscene.h"
#include "mk2/drplayer.h"
#include "dro/param//background/background_data.h"

#include <aoconfig.h>

class SceneManager
{
public:
  SceneManager(const SceneManager&) = delete;

  static SceneManager& get()
  {
    return s_Instance;
  }

  void execLoadPlayerBackground(QString t_backgroundName);
  QString getBackgroundPath(QString t_position);
  QString getForegroundPath(QString t_position);
  DRBackgroundSettings getBackgroundSettings();


  RPLabel *CreateTransition(QWidget *parents, AOApplication* ao_app, DRGraphicsView *viewport);
  RPLabel *GetTransition();

  void RenderTransition();
  void AnimateTransition();
  void setFadeDuration(int duration);

  void clearPlayerDataList();

  QVector<DrPlayer> mPlayerDataList;

  AOConfig *pConfigAO = nullptr;
private:
  SceneManager() {}
  static SceneManager s_Instance;

  int mFadeDuration = 200;
  RPLabel *pUiTransition = nullptr;
  DRGraphicsView *pViewport = nullptr;


  QString mBackgroundName = "";
  BackgroundData *pCurrentBackground = nullptr;

};

#endif // SCENEMANAGER_H
