#ifndef CONFIG_TAB_BLIPS_H
#define CONFIG_TAB_BLIPS_H

#include <QWidget>
#include <rolechat-lib/src/rolechat/audio/blip_config.h>

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

private slots:
  void on_blipSet_currentIndexChanged(int index);

private:
  Ui::ConfigTabBlips *ui;
  std::optional<BlipConfig> m_currentBlip;
};

#endif // CONFIG_TAB_BLIPS_H
