#pragma once

#include "aoobject.h"
#include "draudioengine.h"

// 3rd
#include <bass/bass.h>

// std
#include <optional>

class AOBlipPlayer : public AOObject
{
  Q_OBJECT

public:
  static const int BLIP_COUNT;

  AOBlipPlayer(QObject *p_parent = nullptr);

public slots:
  void set_blips(QString p_sfx);
  void blip_tick();

private:
  DRAudioStreamFamily::ptr m_family;
  std::optional<QString> m_name;
  std::optional<QString> m_file;
};
