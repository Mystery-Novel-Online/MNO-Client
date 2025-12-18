/**************************************************************************
**
** mk2
** Copyright (C) 2022 Tricky Leifa
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Affero General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#pragma once

#include <QObject>
#include <QPixmap>
#include <QSharedPointer>

namespace mk2
{
class SpriteFrame
{
public:
  QImage image;
  int delay = 0;

  SpriteFrame();
  ~SpriteFrame();
};

class SpriteReader : public QObject
{
  Q_OBJECT

public:
  using ptr = QSharedPointer<SpriteReader>;

  enum class State
  {
    NotLoaded,
    FullyLoaded,
  };
  Q_ENUM(State)

  enum class Error
  {
    NoError,
    DeviceError,
    InvalidDataError,
  };
  Q_ENUM(Error)

  static void registerMetatypes();

  explicit SpriteReader(QObject *parent = nullptr);
  virtual ~SpriteReader();

  QString get_file_name() const;

  QIODevice *get_device() const;

  virtual bool is_valid() const;

  virtual QSize get_sprite_size() const;

  virtual int get_frame_count() const;

  virtual SpriteFrame get_frame(int number);

  virtual QVector<SpriteFrame> get_frame_list();

  mk2::SpriteReader::State get_state() const;

  bool is_loaded() const;

  int get_loading_progress() const;

  Error get_last_error() const;

  QImage m_frame_mask;

public slots:
  void set_file_name(QString file_name);

  void set_device(QIODevice *device);

signals:
  void file_name_changed(QString file_name);

  void state_changed(mk2::SpriteReader::State state);

  void loading_progress_changed(int);

  void error(mk2::SpriteReader::Error error);

protected:
  virtual void load();

protected slots:
  void set_state(mk2::SpriteReader::State state);

  void set_loading_progress(int percent);

  void set_error(mk2::SpriteReader::Error error);

private:
  QIODevice *m_device;
  bool m_own_device;
  std::atomic<State> m_state;
  std::atomic<Error> m_last_error;
  std::atomic_int m_loading_progress;

  void _p_delete_device();
};
} // namespace mk2
