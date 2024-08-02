#ifndef REPLAY_HOVER_CONTROLLER_H
#define REPLAY_HOVER_CONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QWidget>
#include <QEvent>

class ReplayHoverController : public QWidget
{
  Q_OBJECT
public:
  explicit ReplayHoverController(QWidget *parent = nullptr);

  void addWidget(QWidget *t_widget)
  {
    m_Widgets.append(t_widget);
    t_widget->setParent(this);
    t_widget->hide();
  };

protected:
  void enterEvent(QEvent *event) override
  {
    for(QWidget * r_widget : m_Widgets) { r_widget->setVisible(true); }
    QWidget::enterEvent(event);
  }

  void leaveEvent(QEvent *event) override
  {
    QTimer::singleShot(300, [this]() { for(QWidget * r_widget : m_Widgets) { r_widget->setVisible(false); } });
    QWidget::leaveEvent(event);
  }
signals:

private:
  QList<QWidget *> m_Widgets = {};
};

#endif // REPLAY_HOVER_CONTROLLER_H
