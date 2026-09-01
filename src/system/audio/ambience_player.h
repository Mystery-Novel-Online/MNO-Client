#pragma once

#include "aoobject.h"
#include "draudioengine.h"


class RPAmbiencePlayer : public AOObject
{
  Q_OBJECT

public:
  RPAmbiencePlayer(QObject *p_parent = nullptr);

public slots:
  void play(QString p_song);
  void toggleReverb(bool reverb);
  void stop();

private:
  DRAudioStreamFamily::ptr m_family;

  DRAudioStream::ptr m_currentAmbience = nullptr;
  DRAudioStream::ptr m_lastAmbience = nullptr;

  QString m_filename;
};
