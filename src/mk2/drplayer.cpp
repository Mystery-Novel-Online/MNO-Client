#include "drplayer.h"
#include "pch.h"

DrPlayer::DrPlayer(int id, QString showname, QString character, QString url, QString status, QString outfit)
{

}

DrPlayer::DrPlayer(JSONReader &jsonReader)
{
  data.id = jsonReader.getStringValue("id").toInt();
  data.showname = jsonReader.getStringValue("showname");
  data.character = jsonReader.getStringValue("character");
  data.contentUrl = jsonReader.getStringValue("url");
  data.status = jsonReader.getStringValue("status");

  data.outfit = jsonReader.getStringValue("outfit");
  if(data.outfit == "<All>")
    data.outfit = "";

  data.modIPID = jsonReader.getStringValue("IPID");
  data.modHDID = jsonReader.getStringValue("HDID");

  data.afk = jsonReader.getStringValue("afk") == "True";
  data.discordSnowflake = jsonReader.getStringValue("discord");
  data.contentVersion = jsonReader.getStringValue("content_version").toInt();
}
