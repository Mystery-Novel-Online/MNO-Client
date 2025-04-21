#ifndef DROEMOTEBUTTONS_H
#define DROEMOTEBUTTONS_H

#include <AOApplication.h>
#include <QObject>
#include <QWidget>

class DROEmoteButtons : public QWidget
{
  Q_OBJECT
public:
  explicit DROEmoteButtons(QWidget *parent);
signals:
protected:
  void wheelEvent(QWheelEvent *event) override;

};

#endif // DROEMOTEBUTTONS_H
