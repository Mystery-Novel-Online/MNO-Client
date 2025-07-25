#include "drplayer.h"
#include "pch.h"

DrPlayer::DrPlayer(int id, QString showname, QString character, QString url, QString status, QString outfit) : m_showname(showname), m_character(character), m_CharacterOutfit(outfit), mURL(url), mPlayerStatus(status)
{
  m_id = id;
}

void DrPlayer::setMod(QString ipid, QString hdid)
{
  mHDID = hdid;
  mIPID = ipid;
}
