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

  void clearPlayerDataList();

  QVector<DrPlayer> m_areaPlayers;

  AOConfig *pConfigAO = nullptr;
private:
  SceneManager() = default;
  static SceneManager s_Instance;

};

#endif // SCENEMANAGER_H
