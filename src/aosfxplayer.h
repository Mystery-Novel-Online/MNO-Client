#pragma once

#include "aoobject.h"
#include "draudiostream.h"
#include "draudiostreamfamily.h"

#include <QMap>
#include <QSharedPointer>

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

  void play_ambient(QString filename);

private:
  DRAudioStreamFamily::ptr m_player;
  QVector<DRAudioStream::ptr> m_stream_list;
  QMap<QString, DRAudioStream::ptr> m_ambient_map;
  DRAudioStream::ptr m_current_ambient;

  DRAudioStream::ptr get_stream_by_qobject(QObject *object);

private slots:
  void remove_ambient();
  void handle_ambient_fade(DRAudioStream::Fade fade);
};
