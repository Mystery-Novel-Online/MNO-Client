#pragma once

#include "aoobject.h"
#include "draudioengine.h"

class AOMusicPlayer : public AOObject
{
  Q_OBJECT

public:
  AOMusicPlayer(AOApplication *p_ao_app, QObject *p_parent = nullptr);

public slots:
  void play(QString p_song);
  void stop();

private:
  DRAudioStreamFamily::ptr m_family;

  DRAudioStream::ptr mCurrentSong = nullptr;
  DRAudioStream::ptr mLastSong = nullptr;

  QString m_filename;
};
