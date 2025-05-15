#ifndef RP_WIDGET_H
#define RP_WIDGET_H

#include <QObject>
#include <QWidget>
#include "dro/interface/widgets/sticker_viewer.h"

class AOApplication;

class RPWidget : public QWidget
{
  Q_OBJECT
public:
  explicit RPWidget(const QString &name, QWidget *parent = nullptr);
  void setDragable(bool dragable);
  void setBackgroundImage(QString imageName);
  virtual void resetTransform();

public:
  AOApplication *m_app = nullptr;

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  QString m_friendlyName = "";
  DRStickerViewer *m_backgroundImage = nullptr;
  QPoint m_dragStartPosition;
  bool m_dragEnabled = false;

signals:
};

#endif // RP_WIDGET_H
