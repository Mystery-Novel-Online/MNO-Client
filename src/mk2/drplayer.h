#ifndef DRPLAYER_H
#define DRPLAYER_H

#include <qstring.h>



class DrPlayer
{
public:
    DrPlayer(int id, QString showname, QString character, QString url, QString status, QString outfit);

    void setMod(QString ipid, QString hdid);

    int m_id;
    QString m_showname;
    QString m_character;
    QString mURL;
    QString mPlayerStatus;
    QString mHDID;
    QString mIPID;
    QString m_CharacterOutfit = "";
};

#endif // DRPLAYER_H
