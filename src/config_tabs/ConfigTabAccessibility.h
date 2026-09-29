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

private:
  Ui::ConfigTabAccessibility *ui;
};

#endif // CONFIGTABACCESSIBILITY_H
