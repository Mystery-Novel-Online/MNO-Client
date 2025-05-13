#ifndef HEALTH_BAR_H
#define HEALTH_BAR_H

#include <QObject>
#include <QWidget>
#include "aoimagedisplay.h"

class HealthBar : public AOImageDisplay
{
  Q_OBJECT
public:
  explicit HealthBar(QString name, AOApplication *app, QWidget *parent = nullptr);

  void SetValue(int value);
  int GetValue() { return m_Value; }

private:
  AOApplication *m_App = nullptr;
  QString m_Name = "";
  int m_HealthIndex = 0;
  int m_Value = 0;

public slots:
  void OnAddClicked();
  void OnSubtractClicked();
};

#endif // HEALTH_BAR_H
