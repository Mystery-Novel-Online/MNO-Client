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

#include "mk2/spritedynamicreader.h"
#include "mk2/spriteviewer.h"

#include <QFile>
#include <QPainter>
#include <QResizeEvent>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "graphicsspriteitem.h"
#include "stb_image_resize2.h"

using namespace mk2;

SpritePlayer::SpritePlayer(QObject *parent)
    : QObject{parent}
    , m_reader{new SpriteDynamicReader}
    , m_scaling_mode{StretchScaling}
    , m_resolved_scaling_mode{StretchScaling}
    , m_transform{Qt::SmoothTransformation}
    , m_running{false}
    , m_mirror{false}
    , m_play_once{false}
    , m_frame_count{0}
    , m_frame_number{0}
{
  m_frame_timer.setTimerType(Qt::PreciseTimer);
  m_frame_timer.setSingleShot(true);

  m_repaint_timer.setInterval(200);
  m_repaint_timer.setSingleShot(true);

  connect(&m_frame_timer, SIGNAL(timeout()), this, SLOT(fetch_next_frame()));
  connect(&m_repaint_timer, SIGNAL(timeout()), this, SLOT(scale_current_frame()));
}

SpritePlayer::~SpritePlayer()
{}

QImage SpritePlayer::get_current_frame() const
{
  return m_scaled_current_frame;
}

QPixmap SpritePlayer::getCurrentPixmap() const
{
  return m_scaled_pixmap_frame;
}

QImage SpritePlayer::get_current_native_frame() const
{
  return m_current_frame.image;
}

QRectF SpritePlayer::get_scaled_bounding_rect() const
{
  return m_scaled_current_frame.rect();
}

QString SpritePlayer::get_file_name() const
{
  return m_reader->get_file_name();
}

QIODevice *SpritePlayer::get_device() const
{
  return nullptr;
}

SpritePlayer::ScalingMode SpritePlayer::get_scaling_mode() const
{
  return m_scaling_mode;
}

QSize SpritePlayer::get_size() const
{
  return m_size;
}

double SpritePlayer::getScaledAmount() const
{
  return m_overallScale;
}

void SpritePlayer::set_file_name(QString p_file_name)
{
  if (!p_file_name.isEmpty() && p_file_name == get_file_name())
  {
    return;
  }
  stop();
  m_reader->set_file_name(p_file_name);
  m_frame_count = m_reader->get_frame_count();
  emit file_name_changed(p_file_name);
}

void SpritePlayer::set_device(QIODevice *p_device)
{
  const QString l_prev_file_name = get_file_name();
  stop();
  m_reader->set_device(p_device);
  m_frame_count = m_reader->get_frame_count();
  const QString l_file_name = get_file_name();
  if (l_file_name != l_prev_file_name)
  {
    emit file_name_changed(l_file_name);
  }
}

void SpritePlayer::set_play_once(bool p_enabled)
{
  m_play_once = p_enabled;
}

void SpritePlayer::set_mirror(bool p_enabled)
{
  m_mirror = p_enabled;
}

void SpritePlayer::set_scaling_mode(ScalingMode scaling_mode)
{
  if (m_scaling_mode == scaling_mode)
  {
    return;
  }
  m_scaling_mode = scaling_mode;
  resolve_scaling_mode();
  scale_current_frame();
}

void SpritePlayer::set_size(QSize p_size)
{
  if (m_size == p_size)
  {
    return;
  }
  m_size = p_size;
  resolve_scaling_mode();
  m_repaint_timer.start();
  emit size_changed(m_size);
}

SpriteReader::ptr SpritePlayer::get_reader() const
{
  return m_reader;
}

void SpritePlayer::set_reader(SpriteReader::ptr p_reader)
{
  const QString l_prev_file_name = get_file_name();
  stop();
  if (p_reader == nullptr)
  {
    p_reader = SpriteReader::ptr(new SpriteDynamicReader);
  }
  m_reader = p_reader;
  m_frame_count = m_reader->get_frame_count();
  const QString l_file_name = get_file_name();
  if (l_file_name != l_prev_file_name)
  {
    emit file_name_changed(l_file_name);
  }
}

bool SpritePlayer::is_valid() const
{
  return m_reader->is_valid();
}

bool SpritePlayer::is_running() const
{
  return m_running;
}

void SpritePlayer::start(ScalingMode scaling, double scale)
{
  m_scale = scale;
  m_manualScalingMode = scaling;
  m_running = true;
  m_elapsed_timer.start();
  emit started();
  resolve_scaling_mode(m_manualScalingMode, m_scale);
  fetch_next_frame();
}

void SpritePlayer::restart()
{
  stop();
  start();
}

void SpritePlayer::stop()
{
  m_running = false;
  if(m_frame_timer.isActive()) m_frame_timer.stop();
  m_frame_number = 0;
}

void SpritePlayer::start(int p_start_frame, ScalingMode scaling, double scale)
{
  m_scale = scale;
  m_manualScalingMode = scaling;
  if(m_frame_count > p_start_frame)
  {
    m_frame_number = p_start_frame;
  }
  else
  {
    m_frame_number = 0;
  }
  m_running = true;
  m_elapsed_timer.start();
  emit started();
  resolve_scaling_mode(m_manualScalingMode, scale);
  fetch_next_frame();
}

void SpritePlayer::restart(int p_start_frame)
{
  stop();
  start(p_start_frame);
}

void SpritePlayer::addLayer(SpriteLayer *layer)
{

  connect(layer->spritePlayerReference(), &SpritePlayer::current_frame_changed, this, &SpritePlayer::scale_current_frame);
  m_layerPlayers.append(layer);
}

void SpritePlayer::clearLayers()
{
  m_layerPlayers.clear();
}

int SpritePlayer::get_frame()
{
  return m_frame_number;
}

void SpritePlayer::resolve_scaling_mode(ScalingMode scalingMode, double scale)
{

  if(scalingMode != AutomaticScaling)
  {
    if(scalingMode == WidthSmoothScaling)
    {
      m_transform = Qt::SmoothTransformation;
      m_resolved_scaling_mode = WidthScaling;
      scale_current_frame();
      return;
    }
    if(scalingMode == WidthPixelScaling)
    {
      m_transform = Qt::FastTransformation;
      m_resolved_scaling_mode = WidthPixelScaling;
      scale_current_frame();
      return;
    }
  }
  m_resolved_scaling_mode = m_scaling_mode;

  const QSize l_image_size = m_reader->get_sprite_size();
  if (m_size == l_image_size || !l_image_size.isValid())
  {
    m_resolved_scaling_mode = NoScaling;
  }
  else if (m_resolved_scaling_mode == DynamicScaling)
  {
    const qreal l_width_factor = (qreal)qMax(l_image_size.width(), 1) / qMax(m_size.width(), 1);
    const qreal l_height_factor = (qreal)qMax(l_image_size.height(), 1) / qMax(m_size.height(), 1);

    const QSize l_by_width_size{
        int((qreal)l_image_size.width() / l_width_factor),
        int((qreal)l_image_size.height() / l_width_factor),
    };
    const QSize l_by_height_size{
        int((qreal)l_image_size.width() / l_height_factor),
        int((qreal)l_image_size.height() / l_height_factor),
    };

    if (l_by_width_size.height() >= m_size.height())
    {
      m_resolved_scaling_mode = WidthScaling;
    }
    else if (l_by_height_size.width() >= m_size.width())
    {
      m_resolved_scaling_mode = HeightScaling;
    }
    else
    {
      m_resolved_scaling_mode = StretchScaling;
    }
  }

  m_transform = Qt::SmoothTransformation;
  if (l_image_size.width() < m_size.width() || l_image_size.height() < m_size.height())
  {
    m_transform = Qt::FastTransformation;
  }
}

void SpritePlayer::fetch_next_frame()
{
  QElapsedTimer l_timer;
  l_timer.start();

  if (!is_valid())
  {
    if (m_running && m_play_once)
    {
      m_running = false;
      emit finished();
    }

    return;
  }

  if (!m_running)
  {
    return;
  }

  if (m_frame_number >= m_frame_count)
  {
    if (m_play_once)
    {
      m_running = false;
      emit finished();
      return;
    }

    if (m_frame_count > 1)
    {
      m_frame_number = 0;
    }
    else
    {
      return;
    }
  }

  const int l_current_frame_number = m_frame_number;
  m_current_frame = m_reader->get_frame(l_current_frame_number);
  m_frame_number++;

  scale_current_frame();

  const int l_next_delay = qMax(0, int(m_current_frame.delay - l_timer.elapsed()));
  if (l_next_delay == 0 && m_frame_count == 1)
  {
    m_running = false;

    if (m_play_once)
    {
      emit finished();
    }
  }
  else
  {
    m_frame_timer.start(l_next_delay);
  }
}

void SpritePlayer::scale_current_frame()
{
  QImage l_image = m_current_frame.image;
  QSizeF originalSize = l_image.size();

  if (l_image.isNull())
    return;

  QImage composed(l_image.size(), QImage::Format_ARGB32_Premultiplied);
  composed.fill(Qt::transparent);

  QPainter combiner(&composed);
  combiner.setRenderHint(QPainter::SmoothPixmapTransform, true);

  QList<SpriteLayer*> overlayQueue = {};

  for (SpriteLayer* layer : m_layerPlayers)
  {
    if (!layer) continue;
    if(layer->layerPosition() != "below")
    {
      overlayQueue.append(layer);
      continue;
    }

    QImage layerImg = layer->spritePlayerReference()->get_current_native_frame();
    if (!layerImg.isNull())
    {
      combiner.drawImage(layer->targetRect.x(), layer->targetRect.y(), layerImg);
    }
  }

  combiner.drawImage(0, 0, l_image);

  for (SpriteLayer* layer : overlayQueue)
  {
    QImage layerImg = layer->spritePlayerReference()->get_current_native_frame();
    if (!layerImg.isNull())
    {
      combiner.drawImage(layer->targetRect, layerImg);
    }
  }

  combiner.end();

  switch (m_resolved_scaling_mode)
  {
  case NoScaling:
  default:
    composed = composed.scaledToWidth(composed.width() * m_scale, m_transform);
    break;
  case StretchScaling:
    composed = composed.scaled(m_size * m_scale, Qt::IgnoreAspectRatio, m_transform);
    break;
  case WidthScaling:
    composed = composed.scaledToWidth(m_size.width() * m_scale, m_transform);
    break;
  case HeightScaling:
    composed = composed.scaledToHeight(m_size.height() * m_scale, m_transform);
    break;

    case WidthPixelScaling:
    {
      const int originalWidth = composed.width();
      if (originalWidth > 0)
      {
        int idealWidth = int(m_size.width() * m_scale);

        int multiples = qMax(1, int((double)idealWidth / originalWidth + 0.5));
        int finalWidth = originalWidth * multiples;
        int finalHeight = composed.height() * multiples;

        composed = composed.scaled(finalWidth, finalHeight, Qt::IgnoreAspectRatio, Qt::FastTransformation);
      }
    }
  }

  m_overallScale = static_cast<double>(composed.width()) / originalSize.width();

  if (m_mirror)
    composed = composed.mirrored(true, false);

  m_scaled_current_frame = composed;
  m_scaled_pixmap_frame = QPixmap::fromImage(m_scaled_current_frame);
  emit current_frame_changed();
}
