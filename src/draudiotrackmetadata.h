#pragma once

#include <QString>

class DRAudiotrackMetadata
{
public:
  static void update_cache();

  DRAudiotrackMetadata();
  DRAudiotrackMetadata(const QString& file_name);
  ~DRAudiotrackMetadata();

  QString filename();
  QString title();
  bool play_once();
  quint64 loop_start();
  quint64 loop_end();

private:
  QString m_filename;
  QString m_title;
  bool m_play_once = false;
  quint64 m_loop_start = 0;
  quint64 m_loop_end = 0;
};
