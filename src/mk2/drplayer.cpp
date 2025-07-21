#include "drplayer.h"
#include "pch.h"

DrPlayer::DrPlayer(int id, QString showname, QString character, QString url, QString status, QString outfit)
{
    m_id = id;
    m_showname = showname;
    m_character = character;
    mURL = url;
    mPlayerStatus = status;
    m_CharacterOutfit = outfit;
}

void DrPlayer::setMod(QString ipid, QString hdid)
{
  mHDID = hdid;
  mIPID = ipid;
}
