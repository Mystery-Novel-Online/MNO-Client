#include "packets_scene.h"
#include <modules/managers/localization_manager.h>
#include <modules/managers/variable_manager.h>
#include "courtroom.h"
#include <aoapplication.h>
#include <modules/globals/dro_math.h>
#include <modules/widgets/investigation_display.h>

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

void PacketJoinedArea::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->reset_viewport();
}

void PacketAreaAmbience::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->set_ambient(t_Contents.at(0));
}

void PacketAreaWeather::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->updateWeather(t_Contents.at(0));
}

void PacketAreaBackground::HandleIncoming(QStringList t_Contents)
{
  DRAreaBackground l_area_bg;
  l_area_bg.background = t_Contents.at(0);

  for (int i = 1; i < t_Contents.size(); ++i)
  {
    const QStringList l_tod_data = t_Contents.at(i).split("|", DR::SkipEmptyParts);
    if (l_tod_data.size() < 2)
      continue;
    l_area_bg.background_tod_map.insert(l_tod_data.at(0), l_tod_data.at(1));
  }

  qDebug() << l_area_bg.background << l_area_bg.background_tod_map;

  AOApplication::getInstance()->m_courtroom->set_background(l_area_bg);
}

void PacketAreaHP::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->set_hp_bar(t_Contents.at(0).toInt(), t_Contents.at(1).toInt());
}

void PacketClock::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->handle_clock(t_Contents.at(1));
}

void PacketTimeOfDay::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->ao_config->set_timeofday(t_Contents.at(0));
}

void PacketGameMode::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->ao_config->set_gamemode(t_Contents.at(0));
}

void PacketPlayAnimation::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->handleAnimation(t_Contents.at(0));
}

void PacketChatTickRate::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->set_tick_rate(t_Contents.at(0).toInt());
}

void PacketICMessage::HandleIncoming(QStringList t_Contents)
{
  if (AOApplication::getInstance()->joined_server())
    AOApplication::getInstance()->m_courtroom->next_chatmessage(t_Contents);
}

void PacketOOCMessage::HandleIncoming(QStringList t_Contents)
{
  QString l_text = t_Contents.at(1);
  if (t_Contents.size() > 2)
  {
    QString l_localization = t_Contents.at(2);
    QString l_varOne = t_Contents.at(3);
    QString l_varTwo = t_Contents.at(4);

    QString l_localized = LocalizationManager::get().getLocalizationText(l_localization, {l_varOne, l_varTwo});
    if(!l_localized.isEmpty()) l_text = l_localized;
  }
  AOApplication::getInstance()->m_courtroom->append_server_chatmessage(t_Contents.at(0), l_text);

}

void PacketSetBGPosition::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->set_character_position(t_Contents.at(0));
}

void PacketSetShowname::HandleIncoming(QStringList t_Contents)
{
  const QString &l_showname = t_Contents.at(0);
  if (AOApplication::getInstance()->ao_config->showname() != l_showname)
    AOApplication::getInstance()->m_courtroom->ignore_next_showname();
  AOApplication::getInstance()->ao_config->set_showname(l_showname);
}

void PacketPlayWTCE::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->handle_wtce(t_Contents.at(0));
}

void PacketCamera::HandleIncoming(QStringList t_Contents)
{
  bool l_ValidID = false;
  int l_CameraID = t_Contents.at(0).toInt(&l_ValidID);

  if(!l_ValidID) return;

  DROCameraDisplay * l_CameraDisplay = ThemeManager::get().GetWidgetType<DROCameraDisplay>("camera_display");

  if(l_CameraDisplay == nullptr) return;


  QString l_PacketData = Base64ToString(t_Contents.at(1));
  QStringList l_MessageData = l_PacketData.split('#');

  qDebug() << l_PacketData;
  if(l_MessageData.at(0) == "MS")
  {
    l_MessageData.removeFirst();
    ICMessageData *l_Message = new ICMessageData(l_MessageData, false);
    l_CameraDisplay->ProcessMessage(l_CameraID, l_Message);
  }
  else if(l_MessageData.at(0) == "SCENE")
  {
    l_MessageData.removeFirst();
    l_CameraDisplay->SetCameraBackground(l_CameraID, l_MessageData.at(1));
    //l_CameraDisplay->ProcessMessage(l_CameraID, l_Message);
  }
}

void PacketInvestigation::HandleIncoming(QStringList t_Contents)
{

  ViewportInvestigationDisplay * l_InvestigationUI = ThemeManager::get().GetWidgetType<ViewportInvestigationDisplay>("investigate_display");
  if(l_InvestigationUI == nullptr) return;
  l_InvestigationUI->ClearObjects();
  QString l_PacketData = Base64ToString(t_Contents.at(0));
  JSONReader l_InvestigationJson = JSONReader();
  l_InvestigationJson.ReadFromString(l_PacketData);


  for (const QJsonValue &value : l_InvestigationJson.mDocument.array())
  {
    if (value.isObject())
    {
      l_InvestigationJson.SetTargetObject(value.toObject());
      QString l_ObjName = l_InvestigationJson.getStringValue("name");
      QString l_ObjDesc = l_InvestigationJson.getStringValue("desc");
      QRect l_ObjRect = l_InvestigationJson.getRectangleValue("rect");

      InvestigationObject *l_NewObject = new InvestigationObject(l_ObjName, l_ObjRect);
      l_NewObject->SetDescription(l_ObjDesc);
      l_InvestigationUI->AddObject(l_NewObject);

    }
  }

}
