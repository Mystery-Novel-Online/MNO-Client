#ifndef CONFIG_TAB_BLIPS_H
#define CONFIG_TAB_BLIPS_H

#include <QWidget>

namespace Ui
{
class config_tab_blips;
}

class config_tab_blips : public QWidget
{
  Q_OBJECT

public:
  explicit config_tab_blips(QWidget *parent = nullptr);
  ~config_tab_blips();

private:
  Ui::config_tab_blips *ui;
};

#endif // CONFIG_TAB_BLIPS_H
