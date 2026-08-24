#ifndef HEALTH_BAR_H
#define HEALTH_BAR_H

class HealthBar : public AOImageDisplay
{
  Q_OBJECT
public:
  explicit HealthBar(QString name, AOApplication *app, QWidget *parent = nullptr);

  void setValue(int value);
  inline int value() const { return m_currentValue; }

public slots:
  void increaseClicked();
  void decreaseClicked();

private:
  AOApplication *m_App = nullptr;
  QString m_Name = "";
  int m_variableIndex = 0;
  int m_currentValue = 0;
};

#endif // HEALTH_BAR_H
