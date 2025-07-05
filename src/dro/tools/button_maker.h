#ifndef BUTTON_MAKER_H
#define BUTTON_MAKER_H

#include <QWidget>
#include <QObject>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>
#include <drgraphicscene.h>
#include <dro/param/actor/actor_loader.h>
#include "drcharactermovie.h"

class ButtonMakerOverlay : public QWidget
{
  Q_OBJECT
public:
  ButtonMakerOverlay(QWidget *parent = nullptr);

public:
  QPoint m_rectPos {0, 0};
  int m_rectSize = 120;
  bool m_dragging = false;
  QPoint m_dragOffset;
  bool m_renderGuides = false;
  QImage m_OverlayImage;

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
};

class ButtonMaker : public QWidget
{
  Q_OBJECT
public:
  ButtonMaker(QWidget *parent = nullptr);
  void SetEmote(DREmote emote);
  void SetCharacter(QString character);

private slots:
  void onGenerateClicked();
  void onAddUnderlayClicked();
  void onAddOverlayClicked();
  void onAlphaClicked();
private:
  QImage LoadImageDialog();
private:

  QVector<DREmote> m_Emotes = {};
  int m_EmoteIndex = 0;

  bool m_IsJson = false;
  QString m_Path = "";

  DRCharacterMovie *m_CharacterSprite = nullptr;
  DRGraphicsView *m_GraphicsView = nullptr;
  ButtonMakerOverlay *m_Overlay = nullptr;

  QImage m_UnderlayImage;
  QImage m_AlphaMaskImage;

};



#endif // BUTTON_MAKER_H
