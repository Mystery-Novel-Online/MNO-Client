#ifndef RP_HOVER_WIDGET_H
#define RP_HOVER_WIDGET_H

#include <QObject>
#include <QTimer>
#include <QWidget>
#include <QEvent>

class RPHoverWidget : public QWidget
{
  Q_OBJECT
public:
  explicit RPHoverWidget(QWidget *parent = nullptr);

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
    for(QWidget * r_widget : m_Widgets) { r_widget->setVisible(false); };
    QWidget::leaveEvent(event);
  }
private:
  QList<QWidget *> m_Widgets = {};
};

#endif // RP_HOVER_WIDGET_H
