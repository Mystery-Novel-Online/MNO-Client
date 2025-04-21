#ifndef JSONPACKET_H
#define JSONPACKET_H

#include <qstring.h>

#include <modules/json/json_reader.h>

class JsonPacket
{
public:
  static void ProcessJson(QString p_jsonString);

private:
  //const static QString PLAYER_LIST_PACKET = "player_list";
  //static const QString NOTIFY_REQUEST_PACKET = "notify_request";

  static void ProcessPlayerListPacket(JSONReader& jsonReader);
  static void ProcessNotifyRequestPacket(JSONReader& jsonReader);
  static void ProcessPairDataPacket(JSONReader& jsonReader);
  static void ProcessPairPacket(JSONReader& jsonReader);
};

#endif // JSONPACKET_H
