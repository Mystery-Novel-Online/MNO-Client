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
#include "graphicsspriteitem.h"

#include <QGraphicsScene>
#include <QPainter>

#include <functional>

#include <modules/managers/scene_manager.h>

using namespace mk2;

GraphicsSpriteItem::GraphicsSpriteItem(QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_player(new SpritePlayer)
{
  connect(m_player.get(), SIGNAL(size_changed(QSize)), this, SLOT(notify_size()));
  connect(m_player.get(), SIGNAL(current_frame_changed()), this, SLOT(notify_update()));
  connect(m_player.get(), SIGNAL(file_name_changed(QString)), this, SIGNAL(file_name_changed(QString)));
  connect(m_player.get(), SIGNAL(reader_changed()), this, SIGNAL(reader_changed()));
  connect(m_player.get(), SIGNAL(started()), this, SIGNAL(started()));
  connect(m_player.get(), SIGNAL(finished()), this, SIGNAL(finished()));
}

GraphicsSpriteItem::~GraphicsSpriteItem()
{}

SpritePlayer::ScalingMode GraphicsSpriteItem::get_scaling_mode() const
{
  return m_player->get_scaling_mode();
}

void GraphicsSpriteItem::set_scaling_mode(SpritePlayer::ScalingMode p_scaling_mode)
{
  m_player->set_scaling_mode(p_scaling_mode);
}

QSizeF GraphicsSpriteItem::get_size() const
{
  return m_player->get_size();
}

void GraphicsSpriteItem::set_size(QSizeF p_size)
{
  m_player->set_size(p_size.toSize());
}

void GraphicsSpriteItem::set_play_once(bool p_enabled)
{
  m_player->set_play_once(p_enabled);
}

void GraphicsSpriteItem::set_mirror(bool p_enabled)
{
  m_player->set_mirror(p_enabled);
}

QString GraphicsSpriteItem::get_file_name() const
{
  return m_player->get_file_name();
}

void GraphicsSpriteItem::set_file_name(QString p_file_name)
{
  m_player->set_file_name(p_file_name);
}

QIODevice *GraphicsSpriteItem::get_device() const
{
  return m_player->get_device();
}

void GraphicsSpriteItem::set_device(QIODevice *p_device)
{
  m_player->set_device(p_device);
}

SpriteReader::ptr GraphicsSpriteItem::get_reader() const
{
  return m_player->get_reader();
}

void GraphicsSpriteItem::set_reader(SpriteReader::ptr p_reader)
{
  m_player->set_reader(p_reader);
}

SpritePlayer *GraphicsSpriteItem::get_player() const
{
  return m_player.get();
}

void GraphicsSpriteItem::start()
{
  m_player->start();
}

void GraphicsSpriteItem::restart()
{
  stop();
  start();
}

void GraphicsSpriteItem::setVerticalOffset(int t_offset)
{
  mVerticalVPOffset = -t_offset;
}

void GraphicsSpriteItem::setBackgroundScaling(double t_offset)
{
  //TO-DO
  //DRBackgroundSettings l_settings = SceneManager::get().getBackgroundSettings();
  //double l_minimum_scale = l_settings.mScaleMinimum;
  //double l_maxScale = l_settings.mScaleMax;
  //
  //double l_minimumPosition = l_settings.mPositionMinimum;
  //double l_maxPosition = l_settings.mPositionMaximum;

}

void GraphicsSpriteItem::stop()
{
  m_player->stop();
}

bool GraphicsSpriteItem::is_valid() const
{
  return m_player->is_valid();
}

bool GraphicsSpriteItem::is_running() const
{
  return m_player->is_running();
}

QRectF GraphicsSpriteItem::boundingRect() const
{
  return QRectF(0, 0, m_player->get_size().width(), m_player->get_size().height());
}

void GraphicsSpriteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  const QImage l_image = m_player->get_current_frame();
  if (!l_image.isNull())
  {
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    // calculate center position
    QPointF l_horizontal_center;
    if (auto *l_scene = scene())
    {
      const QPointF l_center = l_scene->sceneRect().center() - m_player->get_scaled_bounding_rect().center();
      l_horizontal_center.setX(l_center.x());
      l_horizontal_center.setY(mVerticalVPOffset);
    }

    painter->drawImage(l_horizontal_center, m_player->get_current_frame());
    painter->restore();
  }
}

void GraphicsSpriteItem::notify_size()
{
  emit size_changed(QSizeF(m_player->get_size()));
}

void GraphicsSpriteItem::notify_update()
{
  update();
}
