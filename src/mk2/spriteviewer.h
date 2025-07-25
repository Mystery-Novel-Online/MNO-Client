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

#include "spriteplayer.h"
#include "spritereader.h"

#include <QLabel>
#include <QScopedPointer>

namespace mk2
{
class SpriteViewer : public QLabel
{
  Q_OBJECT

public:
  using ScalingMode = rolechat::actor::ActorScalingMode;

  SpriteViewer(QWidget *parent = nullptr);
  ~SpriteViewer();

  rolechat::actor::ActorScalingMode get_scaling_mode() const;

  QString get_file_name() const;

  QIODevice *get_device() const;

  SpriteReader::ptr get_reader() const;

  SpritePlayer *get_player() const;

  bool is_valid() const;

  bool is_running() const;

  int get_frame();

public slots:
  void set_scaling_mode(rolechat::actor::ActorScalingMode scaling_mode);

  void set_play_once(bool enabled);

  void set_mirror(bool enabled);

  void set_file_name(QString file_name);

  void set_device(QIODevice *device);

  void set_reader(SpriteReader::ptr reader);

  void start();
  void restart();
  void stop();

  void restart(int p_start_frame);
  void start(int p_start_frame);

signals:
  void file_name_changed(QString);

  void reader_changed();

  void started();
  void finished();

protected:
  void resizeEvent(QResizeEvent *event) final;

private:
  QScopedPointer<SpritePlayer> m_player;

private slots:
  void paint_frame();
};
} // namespace mk2
