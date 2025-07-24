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
#include <QPainter>

#include "engine/animation/keyframe_sequence.h"

class SpriteLayer
{
public:
  SpriteLayer(QString name, const QRectF &rect);
  SpriteLayer(QMap<ViewportSprite, mk2::SpriteReader::ptr> &readerMap, const QRectF &rect, ViewportSprite state);
  ~SpriteLayer();
  void start(double scale);
  const QString& name();
  const QString& layerPosition();
  bool detatched();
  QPainter::CompositionMode compositionMode();

  void setState(ViewportSprite state);
  void setName(const QString& name);
  void setLayerPositioning(const QString& name);
  void setDetatch(bool state);
  void setCompositionMode(QPainter::CompositionMode mode);

  QPixmap &getPixmap(bool renderAllowed = false);

  mk2::SpritePlayer* spritePlayerReference();


  mk2::SpritePlayer spritePlayer;
  QRectF targetRect;

private:

  QMap<ViewportSprite, mk2::SpriteReader::ptr> m_readerMapping = {};
  ViewportSprite m_ViewportState = ViewportCharacterIdle;


  bool m_detatch = false;
  QString m_name = "";
  QString m_layerPosition = "";
  double m_currentScale = 0.0f;
  QPainter::CompositionMode m_compositionMode = QPainter::CompositionMode_SourceOver;

  QPixmap m_screenshotPixmap = QPixmap(0, 0);
};

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

  void setLayerState(ViewportSprite viewportState);

  void processOverlays(const QString &overlayString, const QString& character, const QString& emotePath, const QString& outfitName);
  void processOverlays(const QVector<ActorLayer>& ActorLayers, const QString& character, const QString& emotePath, const QString& outfitName);
  SpriteLayer *createOverlay(const QString &characterName, const QString &emoteName, const QString &outfitName, const QStringList &layerStrings);
  SpriteLayer *createOverlay(const QString &imageName, const QString &imageOrder, QRectF rect, const QString &layerName, bool detatched = false);
  SpriteLayer *createOverlay(const ActorLayer& layer, const QString &imagePath);
  void clearImageLayers();


  QPointF computeDrawPosition(const QVector3D &animationOffset) const;
  void drawSpriteLayers(QPainter *painter, QVector<SpriteLayer*> &layers, const QPointF &basePos, double scale, const std::unordered_map<std::string, QVariant>& evaluatedFrames, double alpha);
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) final;
  void paintGL();

public slots:
  void set_scaling_mode(SpritePlayer::ScalingMode scaling_mode);

  void set_size(QSizeF size);

  void set_play_once(bool enabled);

  void set_mirror(bool enabled);

  void set_file_name(QString file_name);

  void set_device(QIODevice *device);

  void set_reader(SpriteReader::ptr reader);

  void start(ScalingMode scaling = ScalingMode::AutomaticScaling, double scale = 1.0f);
  void restart();
  void setVerticalOffset(int t_offset);
  void setHorizontalOffset(int t_offset);
  bool setKeyframeAnimation(const QString &directory, const QString &animation);
  bool setThemeAnimation(const QString &animation);
  bool setCharacterAnimation(QString name, QString character, bool startFromEnd = false);
  void stop();

  void setFlipped(bool state);
  void setMirrored(bool state);
  bool mirroredState();

signals:
  void size_changed(QSizeF);

  void file_name_changed(QString);

  void reader_changed();

  void started();
  void finished();

private:
  ViewportSprite m_spriteState = ViewportCharacterIdle;
  QVector<SpriteLayer*> m_spriteLayersStatic;
  QVector<SpriteLayer*> m_spriteLayers;
  QVector<SpriteLayer*> m_spriteLayersBelow;
  QScopedPointer<SpritePlayer> m_player;
  KeyframeSequence m_KeyframeSequence;
  int mVerticalVPOffset = 0;
  int m_HorizontalOffset = 0;
  bool m_LayersExist = false;

  bool m_isMirrored = false;
  bool m_isFlipped = false;

private slots:
  void notify_size();

  void notify_update();
};
} // namespace mk2
