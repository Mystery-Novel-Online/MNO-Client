#ifndef CONFIG_TAB_BLIPS_H
#define CONFIG_TAB_BLIPS_H

#include <QWidget>

namespace Ui
{
class ConfigTabBlips;
}

class config_tab_blips : public QWidget
{
  Q_OBJECT

public:
  explicit config_tab_blips(QWidget *parent = nullptr);
  ~config_tab_blips();

private:
  Ui::ConfigTabBlips *ui;
};

#endif // CONFIG_TAB_BLIPS_H
