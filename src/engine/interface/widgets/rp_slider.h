#ifndef RP_SLIDER_H
#define RP_SLIDER_H

#include <QObject>
#include <QSlider>
#include <QWidget>

class RPSlider : public QSlider
{
  Q_OBJECT
public:
  RPSlider(Qt::Orientation orientation, QWidget *parent);
};

#endif // RP_SLIDER_H
