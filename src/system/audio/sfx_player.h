#pragma once

#include "aoobject.h"
#include "draudiostream.h"
#include "draudiostreamfamily.h"

#include <QVector>

class AOSfxPlayer : public AOObject
{
  Q_OBJECT

public:
  static const int DEFAULT_FADE_DURATION = 5000;

  AOSfxPlayer(QObject *parent = nullptr);

  void play(QString filename);
  void play_effect(QString effect);
  void play_character_effect(QString character, QString effect);
  void stop_all();

private:
  DRAudioStreamFamily::ptr m_player;
  QVector<DRAudioStream::ptr> m_stream_list;
};