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
#include "dro/fs/fs_reading.h"

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
  for(const EmoteLayer &layer : AnimationReader(name, m_KeyframeSequence, character).m_Layers)
  {
    QString filePath = FS::Paths::FindFile("characters/" + character + "/animations/assets/" + layer.offsetName + ".png");
    if(!FS::Checks::FileExists(filePath)) filePath = FS::Paths::FindFile("animations/assets/" + layer.offsetName + ".png");
    if(!FS::Checks::FileExists(filePath)) AOApplication::getInstance()->get_character_sprite_idle_path(character, layer.offsetName);

    createOverlay(filePath, layer.spriteOrder, layer.layerOffset, layer.offsetName, layer.detachLayer);
  }

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

void GraphicsSpriteItem::processOverlays(const QString &overlayString, const QString& character, const QString& emotePath, const QString& outfitName)
{
  clearImageLayers();

  QString path = QFileInfo(emotePath).path();
  if (!path.isEmpty()) path += "/";

  for(const QString& layerOffset : dro::system::encoding::text::DecodeBase64(overlayString))
  {
    QStringList offsetData = dro::system::encoding::text::DecodePacketContents(layerOffset);
    if(offsetData.length() == 7)
    {
      QString filePath = AOApplication::getInstance()->get_character_sprite_idle_path(character, path + offsetData[0]);
      if(!outfitName.isEmpty())
      {
        const QString currentOutfitName = AOApplication::getInstance()->get_character_sprite_idle_path(character, "outfits/" + outfitName +  "/" + offsetData[0]);
        if(FS::Checks::FileExists(currentOutfitName)) filePath = currentOutfitName;
      }
      else
      {
        const QString currentOutfitName = AOApplication::getInstance()->get_character_sprite_idle_path(character, offsetData[0]);
        if(FS::Checks::FileExists(currentOutfitName)) filePath = currentOutfitName;
      }
      createOverlay(filePath, offsetData[1], QRectF(offsetData[2].toInt(), offsetData[3].toInt(), offsetData[4].toInt(), offsetData[5].toInt()), offsetData[6]);
    }
  }
}

void GraphicsSpriteItem::processOverlays(const QVector<EmoteLayer> &emoteLayers, const QString& character, const QString& emotePath, const QString& outfitName)
{
  clearImageLayers();

  QString path = QFileInfo(emotePath).path();
  if (!path.isEmpty()) path += "/";

  for(const EmoteLayer &layer : emoteLayers)
  {
    QString filePath = AOApplication::getInstance()->get_character_sprite_idle_path(character, path + layer.spriteName);
    if(!outfitName.isEmpty())
    {
      const QString currentOutfitName = AOApplication::getInstance()->get_character_sprite_idle_path(character, "outfits/" + outfitName +  "/" + layer.spriteName);
      if(FS::Checks::FileExists(currentOutfitName)) filePath = currentOutfitName;
    }
    else
    {
      const QString currentOutfitName = AOApplication::getInstance()->get_character_sprite_idle_path(character, layer.spriteName);
      if(FS::Checks::FileExists(currentOutfitName)) filePath = currentOutfitName;
    }
    createOverlay(filePath, layer.spriteOrder, layer.layerOffset, layer.offsetName, layer.detachLayer);
  }

}

void GraphicsSpriteItem::createOverlay(const QString &imageName, const QString &imageOrder, QRectF rect, const QString &layerName, bool detatched)
{
  mk2::SpriteReader::ptr l_new_reader;
  l_new_reader = mk2::SpriteReader::ptr(new mk2::SpriteSeekingReader);
  l_new_reader->set_file_name(imageName);

  if(detatched)
  {
    const QRectF &normRect = rect;
    const QRectF sceneRect = scene()->sceneRect();
    rect.setLeft((normRect.left()) * sceneRect.width() / 1000.0);
    rect.setTop((normRect.top()) * sceneRect.height() / 1000.0);
    rect.setWidth(normRect.width() * sceneRect.width() / 1000.0);
    rect.setHeight(normRect.height() * sceneRect.height() / 1000.0);
  }

  SpriteLayer *layer = new SpriteLayer(imageName, rect);
  layer->setName(layerName);
  layer->setDetatch(detatched);

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
  }

  for(SpriteLayer *layer : m_spriteLayersBelow)
  {
    delete layer;
  }
  m_spriteLayers.clear();
  m_spriteLayersBelow.clear();
  update();
}

QPointF GraphicsSpriteItem::computeDrawPosition(const QVector3D &animationOffset) const
{
  if (!scene())
    return QPointF();

  const QRectF sceneRect = scene()->sceneRect();
  const QRectF scaledRect = m_player->get_scaled_bounding_rect();
  const QPointF centerOffset = sceneRect.center() - scaledRect.center();

  const float verticalOffset = (mVerticalVPOffset / 1000.0f) * (sceneRect.height() + scaledRect.height()) / 2.0f;
  const float horizontalOffset = (m_HorizontalOffset / 1000.0f) * (sceneRect.width() + scaledRect.width()) / 2.0f;

  return centerOffset + QPointF(horizontalOffset + animationOffset.x(),
                                verticalOffset + animationOffset.y());
}

void GraphicsSpriteItem::drawSpriteLayers(QPainter *painter, QVector<SpriteLayer*> &layers, const QPointF &basePos, double scale, const std::unordered_map<std::string, QVariant>& evaluatedFrames, double alpha)
{
  for (SpriteLayer *layer : layers)
  {
    layer->start(scale);
    const QImage frame = layer->spritePlayer.get_current_frame();
    if (frame.isNull())
      continue;


    double alphaValue = alpha;
    double scaleValue = layer->detatched() ? 1.0 : scale;

    QPointF basePosition = layer->detatched() ? QPointF(0, 0) : basePos;
    QVector3D newPosition;
    std::string posChannelName = QString(layer->name() + "_position").toStdString();
    std::string alphChannelName = QString(layer->name() + "_alpha").toStdString();

    if (auto it = evaluatedFrames.find(posChannelName); it != evaluatedFrames.end())
      newPosition = it->second.value<QVector3D>();

    if (auto it = evaluatedFrames.find(alphChannelName); it != evaluatedFrames.end())
    {
      alphaValue = it->second.toFloat();
      if(alphaValue == 0.0f) continue;
      painter->setOpacity(alphaValue);
    }

    QRectF finalRect;

    QRectF scaledRect(layer->targetRect);
    scaledRect.setRect((scaledRect.left() + newPosition.x()) * scaleValue,
                       (scaledRect.top() + newPosition.y()) * scaleValue,
                       scaledRect.width() * scaleValue,
                       scaledRect.height() * scaleValue);
    scaledRect.moveTopLeft(scaledRect.topLeft() + basePosition);
    finalRect = scaledRect;

    painter->drawImage(finalRect, frame);
  }
}

void GraphicsSpriteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  const QImage baseImage = m_player->get_current_frame();

  //Check if the main sprite is valid, otherwise no reason to paint.
  if (baseImage.isNull()) return;

  const double scale = m_player->getScaledAmount();
  float alpha = 1.0f;
  float animScale = 1.0f;
  float rotation = 0.0f;
  QVector3D animationOffset;

  painter->save();
  painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

  std::unordered_map<std::string, QVariant> evaluatedValues;
  m_KeyframeSequence.Evaluate(evaluatedValues);

  if (auto it = evaluatedValues.find("alpha"); it != evaluatedValues.end())
    alpha = it->second.toFloat();
  if (auto it = evaluatedValues.find("position"); it != evaluatedValues.end())
    animationOffset = it->second.value<QVector3D>();
  if (auto it = evaluatedValues.find("rotation"); it != evaluatedValues.end())
    rotation = it->second.toFloat();
  if (auto it = evaluatedValues.find("scale"); it != evaluatedValues.end())
    animScale = it->second.toFloat();

  if (alpha == 0.0f)
  {
    painter->restore();
    return;
  }

  painter->setOpacity(alpha);
  painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

  const QRectF sceneRect = scene()->sceneRect();
  QPointF pivot(sceneRect.center().x(), sceneRect.bottom());

  painter->translate(pivot);
  painter->scale(animScale, animScale);
  painter->rotate(rotation);
  painter->translate(-pivot);

  const QPointF drawPos = computeDrawPosition(animationOffset);

  drawSpriteLayers(painter, m_spriteLayersBelow, drawPos, scale, evaluatedValues, alpha);

  painter->drawImage(drawPos, baseImage);

  drawSpriteLayers(painter, m_spriteLayers, drawPos, scale, evaluatedValues, alpha);

  painter->restore();
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

SpriteLayer::~SpriteLayer()
{
  spritePlayer.set_file_name("");
  spritePlayer.start();
}

void SpriteLayer::start(double scale)
{
  if(m_currentScale == scale) return;
  spritePlayer.start(SpritePlayer::WidthSmoothScaling, scale);
  m_currentScale = scale;
}

const QString &SpriteLayer::name()
{
  return m_name;
}

void SpriteLayer::setName(const QString &name)
{
  m_name = name;
}

bool SpriteLayer::detatched()
{
  return m_detatch;
}

void SpriteLayer::setDetatch(bool state)
{
  m_detatch = state;
}
