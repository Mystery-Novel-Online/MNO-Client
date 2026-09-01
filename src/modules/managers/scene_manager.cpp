#include "scene_manager.h"
#include "pch.h"

#include "fs/fs_reading.h"
#include "param/background/legacy_background_reader.h"
#include <modules/theme/legacythememanager.h>
#include "rolechat/background/JsonBackgroundData.h"

SceneManager SceneManager::s_Instance;

void SceneManager::clearPlayerDataList()
{
  m_areaPlayers.clear();
}
