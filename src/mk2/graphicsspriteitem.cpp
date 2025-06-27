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
#include <QOpenGLContext>
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
#include "dro/system/runtime_loop.h"

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
  RuntimeLoop::setPause(true);
  m_player->start(scaling, scale);
  RuntimeLoop::setPause(false);
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

bool GraphicsSpriteItem::setKeyframeAnimation(const QString &directory, const QString &animation)
{
  clearImageLayers();
  for(const EmoteLayer &layer : AnimationReader(directory + "/" + animation, m_KeyframeSequence).m_Layers)
  {
    QString filePath = FS::Paths::FindFile("animations/" + directory + "/assets/" + layer.offsetName, true, FS::Formats::SupportedImages());

    if(!FS::Checks::FileExists(filePath))
      filePath = FS::Paths::FindFile("animations/assets/" + layer.offsetName, true, FS::Formats::SupportedImages());

    createOverlay(layer, filePath);
  }

  return m_KeyframeSequence.getLoopState();
}

bool GraphicsSpriteItem::setCharacterAnimation(QString name, QString character, bool startFromEnd)
{
  for(const EmoteLayer &layer : AnimationReader(name, m_KeyframeSequence, character).m_Layers)
  {
    QString filePath = FS::Paths::FindFile("characters/" + character + "/animations/assets/" + layer.offsetName, true, FS::Formats::SupportedImages());
    if(!FS::Checks::FileExists(filePath))
      filePath = FS::Paths::FindFile("animations/assets/" + layer.offsetName, true, FS::Formats::SupportedImages());
    if(!FS::Checks::FileExists(filePath))
      filePath = AOApplication::getInstance()->get_character_sprite_idle_path(character, layer.offsetName);

    createOverlay(layer, filePath);
  }

  if(startFromEnd) m_KeyframeSequence.SequenceJumpEnd();

  return m_KeyframeSequence.getLoopState();
}

void GraphicsSpriteItem::stop()
{
  m_player->stop();
}

void GraphicsSpriteItem::setFlipped(bool state)
{
  m_isFlipped = state;
}

void GraphicsSpriteItem::setMirrored(bool state)
{
  m_isMirrored = state;
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

void GraphicsSpriteItem::setLayerState(ViewportSprite viewportState)
{
  m_spriteState = viewportState;
  for(SpriteLayer *layer : m_spriteLayers)
  {
    layer->setState(viewportState);
  }

  for(SpriteLayer *layer : m_spriteLayersBelow)
  {
    layer->setState(viewportState);
  }

}

void GraphicsSpriteItem::processOverlays(const QString &overlayString, const QString& character, const QString& emotePath, const QString& outfitName)
{
  clearImageLayers();

  for(const QString& layerOffset : dro::system::encoding::text::DecodeBase64(overlayString))
  {
    QStringList offsetData = dro::system::encoding::text::DecodePacketContents(layerOffset);
    if(offsetData.length() == 7)
    {
      createOverlay(character, emotePath, outfitName, offsetData);
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
    createOverlay(layer, filePath);
  }

}

void GraphicsSpriteItem::createOverlay(const QString &characterName, const QString &emoteName, const QString &outfitName, const QStringList &layerStrings)
{
  m_LayersExist = true;

  QMap<ViewportSprite, mk2::SpriteReader::ptr> stateSprites =
  {
    {ViewportSprite::ViewportCharacterIdle, mk2::SpriteReader::ptr(new mk2::SpriteSeekingReader)},
    {ViewportSprite::ViewportCharacterTalk, mk2::SpriteReader::ptr(new mk2::SpriteSeekingReader)}
  };

  QRectF rect = QRectF(layerStrings[2].toInt(), layerStrings[3].toInt(), layerStrings[4].toInt(), layerStrings[5].toInt());

  QString path = QFileInfo(emoteName).path();
  if (!path.isEmpty()) path += "/";

  //Create the fallback path.


  QString filePath = FS::Paths::FindFile("characters/" + characterName + "/" + path + layerStrings[0], true, FS::Formats::AnimatedImages());
  if(!FS::Checks::FileExists(filePath))
  {
    filePath = FS::Paths::FindFile("characters/" + characterName + "/outfits/" + outfitName + "/" + layerStrings[0], true, FS::Formats::AnimatedImages());
    if(!FS::Checks::FileExists(filePath))
    {
      filePath = AOApplication::getInstance()->get_character_sprite_idle_path(characterName, layerStrings[0]);
    }
  }
  stateSprites[ViewportCharacterIdle]->set_file_name(filePath);

  const QString prefixedName = "(b)" + layerStrings[0];
  QString talkingFilepath = FS::Paths::FindFile("characters/" + characterName + "/" + path + prefixedName, true, FS::Formats::AnimatedImages());
  if(!FS::Checks::FileExists(talkingFilepath))
  {
    talkingFilepath = FS::Paths::FindFile("characters/" + characterName + "/outfits/" + outfitName + "/" + prefixedName, true, FS::Formats::AnimatedImages());
    if(!FS::Checks::FileExists(talkingFilepath))
    {
      talkingFilepath = FS::Paths::FindFile("characters/" + characterName + "/" + prefixedName, true, FS::Formats::AnimatedImages());
    }
  }

  if(FS::Checks::FileExists(talkingFilepath))
    stateSprites[ViewportCharacterTalk]->set_file_name(talkingFilepath);
  else
    stateSprites[ViewportCharacterTalk]->set_file_name(filePath);

  SpriteLayer *layer = new SpriteLayer(stateSprites, rect, m_spriteState);
  layer->setName(layerStrings[0]);
  layer->setDetatch(false);

  if(layerStrings[1].toLower() == "below")
  {
    m_spriteLayersBelow.append(layer);
  }
  else
  {
    m_spriteLayers.append(layer);
  }
  update();
}

void GraphicsSpriteItem::createOverlay(const QString &imageName, const QString &imageOrder, QRectF rect, const QString &layerName, bool detatched)
{
  m_LayersExist = true;

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

void GraphicsSpriteItem::createOverlay(const EmoteLayer &layer, const QString &imagePath)
{
  m_LayersExist = true;

  QRect targetRect = layer.layerOffset;
  if(layer.detachLayer)
  {
    const QRectF &normRect = layer.layerOffset;
    const QRectF sceneRect = scene()->sceneRect();
    targetRect.setLeft((normRect.left()) * sceneRect.width() / 1000.0);
    targetRect.setTop((normRect.top()) * sceneRect.height() / 1000.0);
    targetRect.setWidth(normRect.width() * sceneRect.width() / 1000.0);
    targetRect.setHeight(normRect.height() * sceneRect.height() / 1000.0);
  }

  SpriteLayer *layerData = new SpriteLayer(imagePath, targetRect);
  layerData->setName(layer.offsetName);
  layerData->setDetatch(layer.detachLayer);

  static const QMap<QString, QPainter::CompositionMode> compositionTable
  {
    {"source_over", QPainter::CompositionMode_SourceOver},
    {"destination_over", QPainter::CompositionMode_DestinationOver},
    {"clear", QPainter::CompositionMode_Clear},
    {"source", QPainter::CompositionMode_Source},
    {"destination", QPainter::CompositionMode_Destination},
    {"source_in", QPainter::CompositionMode_SourceIn},
    {"destination_in", QPainter::CompositionMode_DestinationIn},
    {"source_out", QPainter::CompositionMode_SourceOut},
    {"destination_out", QPainter::CompositionMode_DestinationOut},
    {"source_atop", QPainter::CompositionMode_SourceAtop},
    {"destination_atop", QPainter::CompositionMode_DestinationAtop},
    {"xor", QPainter::CompositionMode_Xor},

    {"plus", QPainter::CompositionMode_Plus},
    {"multiply", QPainter::CompositionMode_Multiply},
    {"screen", QPainter::CompositionMode_Screen},
    {"overlay", QPainter::CompositionMode_Overlay},
    {"darken", QPainter::CompositionMode_Darken},
    {"lighten", QPainter::CompositionMode_Lighten},
    {"color_dodge", QPainter::CompositionMode_ColorDodge},
    {"color_burn", QPainter::CompositionMode_ColorBurn},
    {"hard_light", QPainter::CompositionMode_HardLight},
    {"soft_light", QPainter::CompositionMode_SoftLight},
    {"difference", QPainter::CompositionMode_Difference},
    {"exclusion", QPainter::CompositionMode_Exclusion},

    {"source_or_destination", QPainter::RasterOp_SourceOrDestination},
    {"source_and_destination", QPainter::RasterOp_SourceAndDestination},
    {"source_xor_destination", QPainter::RasterOp_SourceXorDestination},
    {"not_source_and_not_destination", QPainter::RasterOp_NotSourceAndNotDestination},
    {"not_source_or_not_destination", QPainter::RasterOp_NotSourceOrNotDestination},
    {"not_source_xor_destination", QPainter::RasterOp_NotSourceXorDestination},
    {"not_source", QPainter::RasterOp_NotSource},
    {"not_source_and_destination", QPainter::RasterOp_NotSourceAndDestination},
    {"source_and_not_destination", QPainter::RasterOp_SourceAndNotDestination},
    {"not_source_or_destination", QPainter::RasterOp_NotSourceOrDestination},
    {"source_or_not_destination", QPainter::RasterOp_SourceOrNotDestination},
    {"clear_destination", QPainter::RasterOp_ClearDestination},
    {"set_destination", QPainter::RasterOp_SetDestination},
    {"not_destination", QPainter::RasterOp_NotDestination}
  };
  if(compositionTable.contains(layer.blendMode.toLower())) layerData->setCompositionMode(compositionTable.value(layer.blendMode.toLower()));

  if(layer.spriteOrder.toLower() == "below")
  {
    m_spriteLayersBelow.append(layerData);
  }
  else
  {
    m_spriteLayers.append(layerData);
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
  m_LayersExist = false;
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
    QPixmap pixmap = layer->spritePlayer.getCurrentPixmap();
    if (pixmap.isNull())
      continue;

    //layer->start(scale);
    //painter->save();

    const bool isDetached = layer->detatched();
    const double scaleValue = isDetached ? 1.0 : scale;
    const QPointF basePosition = isDetached ? QPointF(0, 0) : basePos;
    const QString name = layer->name();

    // Channel keys
    const std::string posKey = (name + "_position").toStdString();
    const std::string alphaKey = (name + "_alpha").toStdString();
    const std::string scaleKey = (name + "_scale").toStdString();
    const std::string rotKey = (name + "_rotation").toStdString();

    QVector3D newPosition;
    double alphaValue = alpha;
    double animScaleValue = 1.0;
    double animRotationValue = 0.0;


    if (auto it = evaluatedFrames.find(posKey); it != evaluatedFrames.end())
      newPosition = it->second.value<QVector3D>();

    if (auto it = evaluatedFrames.find(alphaKey); it != evaluatedFrames.end())
    {
      alphaValue = it->second.toFloat();
      if (alphaValue == 0.0f)
        continue;
    }

    if (auto it = evaluatedFrames.find(scaleKey); it != evaluatedFrames.end())
      animScaleValue = it->second.toFloat();

    if (auto it = evaluatedFrames.find(rotKey); it != evaluatedFrames.end())
      animRotationValue = it->second.toFloat();


    painter->save();
    painter->setOpacity(alphaValue);
    painter->scale(animScaleValue, animScaleValue);

    QRectF target = layer->targetRect;
    QRectF scaledRect(
        (target.left() + newPosition.x()) * scaleValue,
        (target.top() + newPosition.y()) * scaleValue,
        target.width() * scaleValue,
        target.height() * scaleValue
        );
    scaledRect.moveTopLeft(scaledRect.topLeft() + basePosition);

    const QPointF pivot(scaledRect.center().x(), scaledRect.bottom());

    painter->translate(pivot);
    painter->rotate(animRotationValue);
    painter->translate(-pivot);

    QPainter::CompositionMode mode = layer->compositionMode();
    if(mode != QPainter::CompositionMode_SourceOver) painter->setCompositionMode(mode);
    painter->drawPixmap(scaledRect, pixmap, QRectF(0, 0, pixmap.width(), pixmap.height()));
    painter->restore();
  }
}

void GraphicsSpriteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  QPixmap pixmap = m_player->getCurrentPixmap();
  if (pixmap.isNull()) return;

  const double scale = m_player->getScaledAmount();

  float alpha = 1.0f;
  float animScale = 1.0f;
  float rotation = 0.0f;
  QVector3D animationOffset;

  std::unordered_map<std::string, QVariant> evaluatedValues;
  m_KeyframeSequence.Evaluate(evaluatedValues);

  if (auto it = evaluatedValues.find("alpha"); it != evaluatedValues.end())
    alpha = it->second.toFloat();
  if (alpha == 0.0f) return;

  painter->save();
  painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

  if (auto it = evaluatedValues.find("position"); it != evaluatedValues.end())
    animationOffset = it->second.value<QVector3D>();
  if (auto it = evaluatedValues.find("scale"); it != evaluatedValues.end())
    animScale = it->second.toFloat();
  if (auto it = evaluatedValues.find("rotation"); it != evaluatedValues.end())
    rotation = it->second.toFloat();

  const QRectF sceneRect = scene()->sceneRect();
  const QPointF drawPos = computeDrawPosition(animationOffset);
  const QPointF pivot(sceneRect.center().x() + drawPos.x(), sceneRect.bottom());


  painter->translate(pivot);
  if(m_isMirrored) painter->scale(-animScale, animScale);
  else painter->scale(animScale, animScale);
  painter->rotate(rotation);
  painter->translate(-pivot);
  painter->setOpacity(alpha);

  if(m_LayersExist)
  {
    const QSize targetSize = sceneRect.size().toSize();
    QPixmap pixmapCombined(targetSize);
    //QImage combined(targetSize, QImage::Format_ARGB32_Premultiplied);
    pixmapCombined.fill(Qt::transparent);

    QPainter combiner(&pixmapCombined);
    combiner.setRenderHint(QPainter::SmoothPixmapTransform, true);

    drawSpriteLayers(&combiner, m_spriteLayersBelow, drawPos, scale, evaluatedValues, 1.0);


    combiner.drawPixmap(drawPos, pixmap);

    drawSpriteLayers(&combiner, m_spriteLayers, drawPos, scale, evaluatedValues, 1.0); // alpha=1.0 here

    combiner.end();

    painter->drawPixmap(QPoint(0, 0), pixmapCombined);
  }
  else
  {
    painter->drawPixmap(drawPos, pixmap);
  }
  painter->restore();
}

void GraphicsSpriteItem::paintGL()
{
  qDebug() << QOpenGLContext::currentContext()->format();
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

SpriteLayer::SpriteLayer(QMap<ViewportSprite, mk2::SpriteReader::ptr> &readerMap, const QRectF &rect, ViewportSprite state)
{
  m_readerMapping = readerMap;
  m_ViewportState = state;
  targetRect = rect;

  if(m_readerMapping.contains(state))
  {
    spritePlayer.set_reader(m_readerMapping[m_ViewportState]);
    spritePlayer.set_size(targetRect.size().toSize());
    start(1.0f);
  }
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

QPainter::CompositionMode SpriteLayer::compositionMode()
{
  return m_compositionMode;
}

void SpriteLayer::setState(ViewportSprite state)
{
  m_ViewportState = state;

  if(m_readerMapping.contains(state))
  {
    spritePlayer.stop();
    spritePlayer.set_reader(m_readerMapping[m_ViewportState]);
    spritePlayer.set_size(targetRect.size().toSize());
    spritePlayer.start(SpritePlayer::WidthSmoothScaling, 1.0f);
  }
}

void SpriteLayer::setDetatch(bool state)
{
  m_detatch = state;
}

void SpriteLayer::setCompositionMode(QPainter::CompositionMode mode)
{
  m_compositionMode = mode;
}
