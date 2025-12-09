#ifndef DRPLAYER_H
#define DRPLAYER_H

#include <qstring.h>

struct NovelPlayerData
{
  int id;
  bool afk;
  QString showname;
  QString character;
  QString outfit;
  QString contentUrl;

  QString status;
  QString modHDID;
  QString modIPID;

  QString discordSnowflake;
  int contentVersion;
};

class DrPlayer
{
public:
    DrPlayer(int id, QString showname, QString character, QString url, QString status, QString outfit);

    void setMod(QString ipid, QString hdid);
    void setAfk(bool afkState) { data.afk = afkState; };
    void setDiscord(QString discord) { data.discordSnowflake = discord; };

    int m_id;
    QString m_showname;
    QString m_character;
    QString m_CharacterOutfit = "";
    QString mURL;
    QString mPlayerStatus;
    QString mHDID;
    QString mIPID;
    int m_contentVersion = 0;

    NovelPlayerData data;
};

#endif // DRPLAYER_H
