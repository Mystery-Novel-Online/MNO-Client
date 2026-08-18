#include "drplayer.h"
#include "pch.h"

DrPlayer::DrPlayer(int id, QString showname, QString character, QString url, QString status, QString outfit) : m_showname(showname), m_character(character), m_CharacterOutfit(outfit), mURL(url), mPlayerStatus(status)
{
  m_id = id;
}

DrPlayer::DrPlayer(JSONReader &jsonReader)
{
  m_id = jsonReader.getStringValue("id").toInt();
  m_showname = jsonReader.getStringValue("showname");
  m_character = jsonReader.getStringValue("character");
  mURL = jsonReader.getStringValue("url");
  mPlayerStatus = jsonReader.getStringValue("status");

  m_CharacterOutfit = jsonReader.getStringValue("outfit");
  if(m_CharacterOutfit == "<All>")
    m_CharacterOutfit = "";

  mIPID = jsonReader.getStringValue("IPID");
  mHDID = jsonReader.getStringValue("HDID");

  data.afk = jsonReader.getStringValue("afk") == "True";
  data.discordSnowflake = jsonReader.getStringValue("discord");
  data.contentVersion = jsonReader.getStringValue("content_version").toInt();
}

void DrPlayer::setMod(QString ipid, QString hdid)
{
  mHDID = hdid;
  mIPID = ipid;
}
