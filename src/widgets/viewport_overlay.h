#ifndef VIEWPORTOVERLAY_H
#define VIEWPORTOVERLAY_H

#include "image_display.h"
#include "rp_text_edit.h"

#include <QEnterEvent>
#include <QObject>
#include <QWidget>

class InteractionObject : public QWidget
{
  Q_OBJECT
public:
  explicit InteractionObject(QWidget *parent = nullptr);

  void setName(const QString& name);
  void setDescription(const QString& name);
  void setToggleObject(QWidget* widget);
  void setTextWidget(RPTextEdit* widget);
  void setCursorWidget(AOImageDisplay* widget);

protected:
  void leaveEvent(QEvent *event) override;
  void enterEvent(QEvent *event) override;
  void paintEvent(QPaintEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

private:
  QString m_name;
  QString m_description;
  QWidget* m_targetWidget = nullptr;
  RPTextEdit* m_textWidget = nullptr;
  AOImageDisplay* m_cursor = nullptr;
};


class ViewportOverlay : public QWidget
{
  Q_OBJECT
public:
  explicit ViewportOverlay(QWidget *parent = nullptr);

  void clearInteractions();
  void addInteraction(QRect area, const QString& name, const QString& description);
  void moveCursor(int x, int y);

private:
  QAction *createResizeAction(int t_width, int t_height);
  void resizeWidgetAndChildren(QString t_widget, int t_width, int t_height);

  QList<InteractionObject*> m_Interactions;

  AOImageDisplay *m_CursorOverlay = nullptr;
  AOImageDisplay *m_NameDisplay = nullptr;
  RPTextEdit *m_NameText = nullptr;


protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void enterEvent(QEvent *event) override;

private slots:
  void displayContextMenu(QPoint t_position);
  void detatchViewport();
  void resizeViewport(int t_width, int t_height);

signals:

};

#endif // VIEWPORTOVERLAY_H
