#include "json_packet.h"

#include "engine/param/json_reader.h"
#include "engine/network/metadata/message_metadata.h"
#include "engine/interface/courtroom_layout.h"
#include "modules/managers/notify_manager.h"

using namespace engine::network;

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
    QString discord = jsonReader.getStringValue("discord");
    QString characterName = jsonReader.getStringValue("character");
    QString charaURL = jsonReader.getStringValue("url");
    QString statusPlayer = jsonReader.getStringValue("status");
    QString characterOutfit = jsonReader.getStringValue("outfit");
    bool isAfk = jsonReader.getStringValue("afk") == "True";
    if(characterOutfit == "<All>") characterOutfit = "";

    QString charaIPID = jsonReader.getStringValue("IPID");
    QString charaHDID = jsonReader.getStringValue("HDID");

    DrPlayer* drp = new DrPlayer(playerId, showname, characterName, charaURL, statusPlayer, characterOutfit);
    drp->setMod(charaIPID, charaHDID);
    drp->setAfk(isAfk);
    drp->setDiscord(discord);
    SceneManager::get().mPlayerDataList.append(*drp);
  }
  if(AOApplication::getInstance()->m_courtroom != nullptr)
    AOApplication::getInstance()->m_courtroom->construct_playerlist_layout();
}

void JsonPacket::ProcessNotifyRequestPacket(JSONReader& jsonReader)
{
  jsonReader.SetTargetObject("data");
  NotifyManager::get().SetSenderId(jsonReader.getIntValue("requester_id"));
  //NotifyManager::get().SetSenderCharacter(jsonReader.getStringValue("requester_character"));
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

  const PairMetadata metadata =
  {
    jsonReader.getStringValue("character"),
    jsonReader.getStringValue("outfit"),
    jsonReader.getStringValue("last_sprite"),
    jsonReader.getStringValue("sequence"),
    jsonReader.getStringValue("layers"),
    jsonReader.getIntValue("pair_scale"),
    jsonReader.getIntValue("offset_pair"),
    jsonReader.getIntValue("pair_vertical"),
    jsonReader.getBoolValue("is_leader"),
    jsonReader.getBoolValue("flipped"),
    true
  };
  int offsetSelf = jsonReader.getIntValue("self_offset");
  message::setPairMetadata(metadata, offsetSelf);
}

void JsonPacket::ProcessPairPacket(JSONReader& jsonReader)
{
  jsonReader.SetTargetObject("data");
  int pair_left = jsonReader.getIntValue("pair_left");
  int pair_right = jsonReader.getIntValue("pair_right");

  int localClientId = metadata::user::getClientId();

  if(pair_left == localClientId)
  {
    metadata::user::partner::setPartner(pair_right);
    courtroom::sliders::setHorizontal(jsonReader.getIntValue("offset_left"));
  }

  if(pair_right == localClientId)
  {
    metadata::user::partner::setPartner(pair_left);
    courtroom::sliders::setHorizontal(jsonReader.getIntValue("offset_right"));
  }

}
