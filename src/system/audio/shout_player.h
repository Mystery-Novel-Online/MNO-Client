#ifndef SHOUT_PLAYER_H
#define SHOUT_PLAYER_H

#include "aoobject.h"

class AOShoutPlayer : public AOObject
{
  Q_OBJECT

public:
  AOShoutPlayer(QObject *p_parent = nullptr);

  void play(QString character, QString shout);
};

#endif // SHOUT_PLAYER_H
