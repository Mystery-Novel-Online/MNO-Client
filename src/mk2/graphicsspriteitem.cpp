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

#include <QFileInfo>
#include <QGraphicsScene>
#include <QPainter>
#include <QVector3D>

#include <functional>

#include <modules/managers/scene_manager.h>
#include "dro/param/animation_reader.h"
#include "dro/interface/courtroom_layout.h"
#include "spritedynamicreader.h"
#include "spriteseekingreader.h"
#include "aoapplication.h"
#include "dro/system/text_encoding.h"

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

void GraphicsSpriteItem::start(ScalingMode scaling, double scale)
{
  m_player->start(scaling, scale);
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

void GraphicsSpriteItem::setHorizontalOffset(int t_offset)
{
  m_HorizontalOffset = t_offset - 500;
}

bool GraphicsSpriteItem::setCharacterAnimation(QString name, QString character, bool startFromEnd)
{
  AnimationReader(name, m_KeyframeSequence, character);
  if(startFromEnd) m_KeyframeSequence.SequenceJumpEnd();

  return m_KeyframeSequence.getLoopState();
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
  return QRectF(0, 0, m_player->get_size().width() * 2, m_player->get_size().height() * 2);
}

void GraphicsSpriteItem::processOverlays(const QString &overlayString, const QString& character, const QString& emotePath)
{
  clearImageLayers();

  QString path = QFileInfo(emotePath).path();
  if (!path.isEmpty()) path += "/";

  for(const QString& layerOffset : dro::system::encoding::text::DecodeBase64(overlayString))
  {
    QStringList offsetData = dro::system::encoding::text::DecodePacketContents(layerOffset);
    if(offsetData.length() == 6)
    {
      QString filePath = AOApplication::getInstance()->get_character_sprite_idle_path(character, path + offsetData[0]);
      createOverlay(filePath, offsetData[1], QRectF(offsetData[2].toInt(), offsetData[3].toInt(), offsetData[4].toInt(), offsetData[5].toInt()));
    }
  }
}

void GraphicsSpriteItem::processOverlays(const QVector<EmoteLayer> &emoteLayers, const QString& character, const QString& emotePath)
{
  clearImageLayers();

  QString path = QFileInfo(emotePath).path();
  if (!path.isEmpty()) path += "/";

  for(const EmoteLayer &layer : emoteLayers)
  {
    QString filePath = AOApplication::getInstance()->get_character_sprite_idle_path(character, path + layer.spriteName);
    createOverlay(filePath, layer.spriteOrder, layer.layerOffset);
  }

}

void GraphicsSpriteItem::createOverlay(const QString &imageName, const QString &imageOrder, const QRectF &rect)
{
  mk2::SpriteReader::ptr l_new_reader;
  l_new_reader = mk2::SpriteReader::ptr(new mk2::SpriteSeekingReader);
  l_new_reader->set_file_name(imageName);

  SpriteLayer *layer = new SpriteLayer(imageName, rect);

  if(imageOrder.toLower() == "below")
  {
    m_spriteLayersBelow.append(layer);
  }
  else
  {
    m_spriteLayers.append(layer);
  }
  update();
}

void GraphicsSpriteItem::clearImageLayers()
{
  for(SpriteLayer *layer : m_spriteLayers)
  {
    delete layer;
    m_spriteLayers.removeAll(layer);
  }

  for(SpriteLayer *layer : m_spriteLayersBelow)
  {
    delete layer;
    m_spriteLayersBelow.removeAll(layer);
  }
  update();
}

void GraphicsSpriteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  const QImage l_image = m_player->get_current_frame();
  if (!l_image.isNull())
  {
    double scale = m_player->getScaledAmount();
    float alpha = 1.0f;

    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    // calculate center position
    QPointF l_horizontal_center;
    if (auto *l_scene = scene())
    {
      const QRectF  sceneRect = l_scene->sceneRect();
      const QRectF  scaledRect = m_player->get_scaled_bounding_rect();
      const QPointF center = sceneRect.center() - scaledRect.center();

      float verticalOffsetResult = 0.0f;
      {
        const float t = mVerticalVPOffset / 1000.0f;
        const float maxOffset = sceneRect.height() + scaledRect.height();
        verticalOffsetResult = t * (maxOffset / 2.0f);
      }

      float horizontalOffsetResult = 0.0f;
      {
        const float t = m_HorizontalOffset / 1000.0f;
        const float maxOffset = sceneRect.width() + scaledRect.width();
        horizontalOffsetResult = t * (maxOffset / 2.0f);
      }

      std::unordered_map<std::string, QVariant> evaluatedValues;
      m_KeyframeSequence.Evaluate(evaluatedValues);

      QVector3D animationVector;

      if (evaluatedValues.find("alpha") != evaluatedValues.end())
        alpha = evaluatedValues["alpha"].toFloat();

      if (evaluatedValues.find("position") != evaluatedValues.end())
        animationVector = evaluatedValues["position"].value<QVector3D>();

      painter->setOpacity(alpha);
      l_horizontal_center.setX(center.x() + horizontalOffsetResult + animationVector.x());
      l_horizontal_center.setY(center.y() + verticalOffsetResult + animationVector.y());
    }

    if(alpha != 0.0f)
    {
      if (!m_spriteLayersBelow.isEmpty())
      {
        for (SpriteLayer *layer : m_spriteLayersBelow)
        {
          layer->start(scale);
          if (!layer->spritePlayer.get_current_frame().isNull())
          {
            QRectF drawRect = layer->targetRect;
            drawRect = QRectF(drawRect.left() * scale, drawRect.top() * scale, drawRect.width() * scale, drawRect.height() * scale );
            drawRect.moveTopLeft(drawRect.topLeft() + l_horizontal_center);
            painter->drawImage(drawRect, layer->spritePlayer.get_current_frame());
          }
        }
      }

      painter->drawImage(l_horizontal_center, l_image);

      if (!m_spriteLayers.isEmpty())
      {
        for (SpriteLayer *layer : m_spriteLayers)
        {
          layer->start(scale);
          if (!layer->spritePlayer.get_current_frame().isNull())
          {
            QRectF drawRect = layer->targetRect;
            drawRect = QRectF(drawRect.left() * scale, drawRect.top() * scale, drawRect.width() * scale, drawRect.height() * scale );
            drawRect.moveTopLeft(drawRect.topLeft() + l_horizontal_center);
            painter->drawImage(drawRect, layer->spritePlayer.get_current_frame());
          }
        }
      }
    }

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

SpriteLayer::SpriteLayer(QString name, const QRectF &rect)
{
  mk2::SpriteReader::ptr l_new_reader;
  l_new_reader = mk2::SpriteReader::ptr(new mk2::SpriteSeekingReader);
  l_new_reader->set_file_name(name);
  spritePlayer.set_reader(l_new_reader);
  targetRect = rect;
  spritePlayer.set_size(targetRect.size().toSize());
  start(1.0f);
}

void SpriteLayer::start(double scale)
{
  if(m_currentScale == scale) return;
  spritePlayer.start(SpritePlayer::WidthSmoothScaling, scale);
  m_currentScale = scale;
}
