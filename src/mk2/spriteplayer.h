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

#include "spritereader.h"

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

#include <QElapsedTimer>

class SpriteLayer;
namespace mk2
{
class SpritePlayer : public QObject
{
  Q_OBJECT

public:

  //Q_ENUM(rolechat::actor::ActorScalingMode)

  SpritePlayer(QObject *parent = nullptr);
  ~SpritePlayer();

  QImage get_current_frame() const;
  QPixmap getCurrentPixmap() const;
  QImage get_current_native_frame() const;

  QRectF get_scaled_bounding_rect() const;

  rolechat::actor::ActorScalingMode get_scaling_mode() const;

  QSize get_size() const;
  double getScaledAmount() const;

  QString get_file_name() const;

  QIODevice *get_device() const;

  SpriteReader::ptr get_reader() const;

  bool is_valid() const;

  bool is_running() const;

  int get_frame();

  void addLayer(SpriteLayer* layer);
  void clearLayers();

public slots:
  void set_play_once(bool enabled);

  void set_mirror(bool enabled);

  void set_scaling_mode(rolechat::actor::ActorScalingMode scaling_mode);

  void set_size(QSize size);

  void set_file_name(QString file_name);

  void set_device(QIODevice *device);

  void set_reader(SpriteReader::ptr reader);

  void start(rolechat::actor::ActorScalingMode scaling = rolechat::actor::ActorScalingMode::AutomaticScaling, double scale = 1.0f);
  void restart();
  void stop();

  void start(int p_start_frame, rolechat::actor::ActorScalingMode scaling = rolechat::actor::ActorScalingMode::AutomaticScaling, double scale = 1.0f);
  void restart(int p_start_frame);
signals:
  void current_frame_changed();

  void size_changed(QSize);

  void file_name_changed(QString);

  void reader_changed();

  void started();
  void finished();

private:
  rolechat::actor::ActorScalingMode m_manualScalingMode = rolechat::actor::ActorScalingMode::AutomaticScaling;
  SpriteReader::ptr m_reader;
  SpriteFrame m_current_frame;
  QImage m_scaled_current_frame;
  QPixmap m_scaled_pixmap_frame;
  double m_overallScale = 1.0;
  double m_scale = 1.0;
  rolechat::actor::ActorScalingMode m_scaling_mode;
  rolechat::actor::ActorScalingMode m_resolved_scaling_mode;
  Qt::TransformationMode m_transform;
  QSize m_size;
  bool m_running;
  bool m_mirror;
  bool m_play_once;
  int m_frame_count;
  int m_frame_number;
  QElapsedTimer m_elapsed_timer;
  QTimer m_frame_timer;
  QTimer m_repaint_timer;
  QVector<SpriteLayer*> m_layerPlayers;

  void resolve_scaling_mode(rolechat::actor::ActorScalingMode scalingMode = rolechat::actor::ActorScalingMode::AutomaticScaling, double scale = 1.0f);
public slots:
  void scale_current_frame();
private slots:
  void fetch_next_frame();
};
} // namespace mk2
