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

#include <QGraphicsObject>
#include <QObject>

namespace mk2
{
class GraphicsSpriteItem : public QGraphicsObject
{
  Q_OBJECT

  Q_PROPERTY(QSizeF size READ get_size WRITE set_size NOTIFY size_changed)

public:
  using ScalingMode = SpritePlayer::ScalingMode;

  GraphicsSpriteItem(QGraphicsItem *parent = nullptr);
  ~GraphicsSpriteItem();

  SpritePlayer::ScalingMode get_scaling_mode() const;

  QSizeF get_size() const;

  QString get_file_name() const;

  QIODevice *get_device() const;

  SpriteReader::ptr get_reader() const;

  SpritePlayer *get_player() const;

  bool is_valid() const;

  bool is_running() const;

  QRectF boundingRect() const final;

  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;

public slots:
  void set_scaling_mode(SpritePlayer::ScalingMode scaling_mode);

  void set_size(QSizeF size);

  void set_play_once(bool enabled);

  void set_mirror(bool enabled);

  void set_file_name(QString file_name);

  void set_device(QIODevice *device);

  void set_reader(SpriteReader::ptr reader);

  void start();
  void restart();
  void setVerticalOffset(int t_offset);
  void setBackgroundScaling(double t_offset);
  void stop();

signals:
  void size_changed(QSizeF);

  void file_name_changed(QString);

  void reader_changed();

  void started();
  void finished();

private:
  QScopedPointer<SpritePlayer> m_player;
  int mVerticalVPOffset = 0;

private slots:
  void notify_size();

  void notify_update();
};
} // namespace mk2
