#ifndef RP_WIDGET_H
#define RP_WIDGET_H

#include <QObject>
#include <QWidget>

class AOApplication;

class RPWidget : public QWidget
{
  Q_OBJECT
public:
  explicit RPWidget(QWidget *parent = nullptr);
  AOApplication *m_App = nullptr;

signals:
};

#endif // RP_WIDGET_H
