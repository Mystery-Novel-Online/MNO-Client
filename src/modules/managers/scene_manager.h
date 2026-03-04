#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "src/drgraphicscene.h"
#include "mk2/drplayer.h"
#include "rolechat/viewport_scene.h"

#include <aoconfig.h>

class SceneManager
{
public:
  SceneManager(const SceneManager&) = delete;

  static SceneManager& get()
  {
    return s_Instance;
  }

  RPLabel *CreateTransition(QWidget *parents, AOApplication* ao_app, DRGraphicsView *viewport);
  RPLabel *GetTransition();

  void RenderTransition();
  void AnimateTransition();
  void setFadeDuration(int duration);

  void clearPlayerDataList();

  QVector<DrPlayer> mPlayerDataList;

  AOConfig *pConfigAO = nullptr;
private:
  SceneManager() = default;
  static SceneManager s_Instance;

  int mFadeDuration = 200;
  RPLabel *pUiTransition = nullptr;
  DRGraphicsView *pViewport = nullptr;

};

#endif // SCENEMANAGER_H
