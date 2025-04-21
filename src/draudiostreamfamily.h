#pragma once

#include "draudiostream.h"

#include <QFlags>
#include <QSharedPointer>
#include <QVector>

class DRAudioEngine;
class DRAudioEngineData;

class DRAudioStreamFamily : public QObject
{
  Q_OBJECT

public:
  using ptr = QSharedPointer<DRAudioStreamFamily>;
  using stream_list = QVector<DRAudioStream::ptr>;

  DRAudioStream::ptr create_stream(QString p_file);
  DRAudioStream::ptr play_stream(QString p_file);

  // get
  stream_list get_stream_list() const;
  int32_t get_volume() const;
  int32_t get_capacity() const;
  DRAudio::Options get_options() const;

  // options get
  bool is_suppressed() const;
  bool is_ignore_suppression() const;

public slots:
  void set_volume(int32_t p_volume);
  void set_capacity(int32_t p_capacity);
  void set_options(DRAudio::Options p_options);
  // options setter
  void set_suppressed(bool p_enabled);
  void set_ignore_suppression(bool p_enabled);

signals:
  void volume_changed(int32_t);
  void capacity_changed(int32_t);
  void options_changed(DRAudio::Options);

private:
  friend class DRAudioEngine;
  friend class DRAudioEngineData;
  friend class DRAudioEnginePrivate;

  DRAudio::Family m_family;
  int32_t m_volume = 0;
  int32_t m_capacity = 0;
  DRAudio::Options m_options;
  stream_list m_stream_list;

  DRAudioStreamFamily(DRAudio::Family p_family);

  float calculate_volume();

  void update_capacity();
  void update_options();
  void update_volume();

private slots:
  void on_stream_finished();
};
