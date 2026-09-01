#ifndef DRPLAYER_H
#define DRPLAYER_H

#include <qstring.h>

#include <param/json_reader.h>

struct AreaPlayerData
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
    DrPlayer(JSONReader& jsonReader);
    AreaPlayerData data;
};

#endif // DRPLAYER_H
