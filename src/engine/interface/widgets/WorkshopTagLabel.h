#ifndef WORKSHOPTAGLABEL_H
#define WORKSHOPTAGLABEL_H

#include <QLabel>
#include <QObject>

class WorkshopTagLabel : public QLabel
{
  Q_OBJECT
public:
  explicit WorkshopTagLabel(const QString& text, QWidget* parent = nullptr);

signals:
  void clicked(bool ctrlHeld);

protected:
  void mousePressEvent(QMouseEvent* event) override
  {
    emit clicked(event->modifiers() & Qt::ControlModifier);
    QLabel::mousePressEvent(event);
  }
};

#endif // WORKSHOPTAGLABEL_H
