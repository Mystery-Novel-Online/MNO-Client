#ifndef VIEWPORTOVERLAY_H
#define VIEWPORTOVERLAY_H

#include <QObject>
#include <QWidget>

class ViewportOverlay : public QWidget
{
  Q_OBJECT
public:
  explicit ViewportOverlay(QWidget *parent = nullptr);

private:
  QAction *createResizeAction(int t_width, int t_height);
  void resizeWidgetAndChildren(QString t_widget, int t_width, int t_height);

private slots:
  void displayContextMenu(QPoint t_position);
  void detatchViewport();
  void resizeViewport(int t_width, int t_height);

signals:

};

#endif // VIEWPORTOVERLAY_H
