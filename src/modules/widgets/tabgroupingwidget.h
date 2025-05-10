#ifndef TABGROUPINGWIDGET_H
#define TABGROUPINGWIDGET_H

#include <QObject>
#include <QWidget>
#include <aoimagedisplay.h>

class TabGroupingWidget : public QWidget
{
  Q_OBJECT
public:
  TabGroupingWidget(QWidget *parent = nullptr);
  void setBackgroundImage(QString t_name);
  void setDragable(bool isDragable);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  QPoint m_dragStartPosition;
  bool m_DragEnabled = false;
  AOImageDisplay *w_BackgroundImage = nullptr;

};

#endif // TABGROUPINGWIDGET_H
