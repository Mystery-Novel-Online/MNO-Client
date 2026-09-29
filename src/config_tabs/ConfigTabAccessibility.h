#ifndef CONFIGTABACCESSIBILITY_H
#define CONFIGTABACCESSIBILITY_H

#include <QWidget>

namespace Ui
{
class ConfigTabAccessibility;
}

class ConfigTabAccessibility : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigTabAccessibility(QWidget *parent = nullptr);
  ~ConfigTabAccessibility();

private slots:
  void on_horizontalSlider_valueChanged(int value);

  void on_backgroundDimmingSlider_valueChanged(int value);

private:
  Ui::ConfigTabAccessibility *ui;
};

#endif // CONFIGTABACCESSIBILITY_H
