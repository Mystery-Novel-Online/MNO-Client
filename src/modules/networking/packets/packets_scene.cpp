#include "packets_scene.h"
#include <modules/managers/variable_manager.h>
#include "courtroom.h"
#include <aoapplication.h>

void PacketScene::HandleIncoming(QStringList t_Contents)
{
  while(t_Contents.length() < 10)
  {
    t_Contents.append("");
  }

  VariableManager::get().setVariable("area_name", t_Contents.at(0));
  VariableManager::get().setVariable("map_visual", t_Contents.at(2));
  if (AOApplication::getInstance()->GetCourtroomConstructed()) AOApplication::getInstance()->m_courtroom->handleAnimation("RoomTransition");
}

void NeoPacketScene::HandleIncoming(QStringList t_Contents)
{
  while(t_Contents.length() < 10)
  {
    t_Contents.append("");
  }

  VariableManager::get().setVariable("area_name", t_Contents.at(0));
  VariableManager::get().setVariable("map_visual", t_Contents.at(2));

  if (!AOApplication::getInstance()->GetCourtroomConstructed()) return;
  Courtroom *l_CourtroomScene = AOApplication::getInstance()->m_courtroom;

  l_CourtroomScene->handleAnimation("RoomTransition");


  DRAreaBackground l_area_bg;
  l_area_bg.background = t_Contents.at(1);

  //for (int i = 1; i < l_content.size(); ++i)
  //{
  //  const QStringList l_tod_data = l_content.at(i).split("|", DR::SkipEmptyParts);
  //  if (l_tod_data.size() < 2)
  //    continue;
  //  l_area_bg.background_tod_map.insert(l_tod_data.at(0), l_tod_data.at(1));
  //}

  qDebug() << l_area_bg.background << l_area_bg.background_tod_map;

  l_CourtroomScene->set_background(l_area_bg);

  if(AOApplication::getInstance()->joined_server())
  {
    l_CourtroomScene->handle_song({t_Contents.at(7), "-1", "", "0", "0"});
  }
  l_CourtroomScene->reset_viewport();
}
