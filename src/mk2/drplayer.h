#ifndef DRPLAYER_H
#define DRPLAYER_H

#include <qstring.h>



class DrPlayer
{
public:
    DrPlayer(int id, QString showname, QString character, QString url, QString status, QString outfit);

    void setMod(QString ipid, QString hdid);
    void setAfk(bool afkState) { m_Afk = afkState; };

    int m_id;
    bool m_Afk = false;
    QString m_showname;
    QString m_character;
    QString m_CharacterOutfit = "";
    QString mURL;
    QString mPlayerStatus;
    QString mHDID;
    QString mIPID;
};

#endif // DRPLAYER_H
