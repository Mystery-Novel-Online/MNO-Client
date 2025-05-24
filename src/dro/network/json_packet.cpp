#include "json_packet.h"
#include "qjsonobject.h"
#include "aoapplication.h"
#include "courtroom.h"
#include "dro/param/json_reader.h"
#include "modules/managers/notify_manager.h"
#include "modules/managers/pair_manager.h"
#include <qjsondocument.h>


void JsonPacket::ProcessJson(QString p_jsonString)
{
  if(AOApplication::getInstance()->m_courtroom == nullptr) return;
  JSONReader jsonReader = JSONReader();
  jsonReader.ReadFromString(p_jsonString);

  QString packetValue = jsonReader.getStringValue("packet");

  if(packetValue == "player_list")
  {
    ProcessPlayerListPacket(jsonReader);
  }
  else if(packetValue == "notify_request")
  {
    ProcessNotifyRequestPacket(jsonReader);
  }
  else if(packetValue == "pair_data")
  {
    ProcessPairDataPacket(jsonReader);
  }
  else if(packetValue == "pair")
  {
    ProcessPairPacket(jsonReader);
  }
}

void JsonPacket::ProcessPlayerListPacket(JSONReader& jsonReader)
{
  QJsonArray playerArray = jsonReader.getArrayValue("data");
  SceneManager::get().clearPlayerDataList();
  for(QJsonValueRef ref : playerArray)
  {
    jsonReader.SetTargetObject(ref.toObject());
    int playerId = jsonReader.getStringValue("id").toInt();
    QString showname = jsonReader.getStringValue("showname");
    QString characterName = jsonReader.getStringValue("character");
    QString charaURL = jsonReader.getStringValue("url");
    QString statusPlayer = jsonReader.getStringValue("status");
    QString characterOutfit = jsonReader.getStringValue("outfit");
    if(characterOutfit == "<All>") characterOutfit = "";

    QString charaIPID = jsonReader.getStringValue("IPID");
    QString charaHDID = jsonReader.getStringValue("HDID");

    DrPlayer* drp = new DrPlayer(playerId, showname, characterName, charaURL, statusPlayer, characterOutfit);
    drp->setMod(charaIPID, charaHDID);
    SceneManager::get().mPlayerDataList.append(*drp);
  }
  if(AOApplication::getInstance()->m_courtroom != nullptr)
    AOApplication::getInstance()->m_courtroom->construct_playerlist_layout();
}

void JsonPacket::ProcessNotifyRequestPacket(JSONReader& jsonReader)
{
  jsonReader.SetTargetObject("data");
  NotifyManager::get().SetSenderId(jsonReader.getIntValue("requester_id"));
  NotifyManager::get().SetSenderCharacter(jsonReader.getStringValue("requester_character"));
  NotifyManager::get().SetSenderName(jsonReader.getStringValue("requester_name"));
  NotifyManager::get().SetRequestKey(jsonReader.getStringValue("requester_key"));
  QString requesterType = jsonReader.getStringValue("request_type");

  if(requesterType == "pair")
  {
    NotifyManager::get().SetPairNotifcation();
  }

  NotifyManager::get().ShowNotification();
}

void JsonPacket::ProcessPairDataPacket(JSONReader& jsonReader)
{
  jsonReader.SetTargetObject("data");
  int offsetPair = jsonReader.getIntValue("offset_pair");
  int offsetSelf = jsonReader.getIntValue("self_offset");
  QString charat = jsonReader.getStringValue("character");
  QString emote = jsonReader.getStringValue("last_sprite");
  bool isFlipped = jsonReader.getBoolValue("flipped");
  int offsetVertical = jsonReader.getIntValue("pair_vertical");
  int offsetScale = jsonReader.getIntValue("pair_scale");

  PairManager::get().SetPairData(charat, emote, offsetSelf, offsetPair, isFlipped, offsetScale, offsetVertical);
}

void JsonPacket::ProcessPairPacket(JSONReader& jsonReader)
{
  jsonReader.SetTargetObject("data");
  int pair_left = jsonReader.getIntValue("pair_left");
  int pair_right = jsonReader.getIntValue("pair_right");

  int localClientId = metadata::user::getClientId();

  if(pair_left == localClientId)
    PairManager::get().SetUserPair(pair_right, jsonReader.getIntValue("offset_left"));

  if(pair_right == localClientId)
    PairManager::get().SetUserPair(pair_left, jsonReader.getIntValue("offset_right"));
}
