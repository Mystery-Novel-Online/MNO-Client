#pragma once

#include "aoobject.h"
#include "draudioengine.h"

enum BGMPlayback
{
  BGMPlayback_Standard = 0,
  BGMPlayback_NoFade = 1,
  BGMPlayback_Continue = 2
};


class AOMusicPlayer : public AOObject
{
  Q_OBJECT

public:
  AOMusicPlayer(QObject *p_parent = nullptr);

public slots:
  void customLoopPlay(QString p_song, int loopStart, int loopBack);
  void play(QString p_song, BGMPlayback playbackType = BGMPlayback_Standard);
  void setSpeed(float speed);
  void toggleReverb(bool reverb);
  void setPitch(float speed);
  void stop();

private:
  DRAudioStreamFamily::ptr m_family;

  DRAudioStream::ptr mCurrentSong = nullptr;
  DRAudioStream::ptr mLastSong = nullptr;

  QString m_filename;
};
