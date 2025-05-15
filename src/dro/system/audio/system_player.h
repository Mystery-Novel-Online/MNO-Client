#pragma once

#include "aoobject.h"

class AOSystemPlayer : public AOObject
{
  Q_OBJECT

public:
  AOSystemPlayer(QObject *p_parent = nullptr);

  void play(QString p_file);
};
