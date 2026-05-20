#ifndef CONFIG_TAB_DISCORD_H
#define CONFIG_TAB_DISCORD_H

#include <QWidget>

namespace Ui
{
class ConfigTabDiscord;
}

class ConfigTabDiscord : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigTabDiscord(QWidget *parent = nullptr);
  ~ConfigTabDiscord();

private:
  Ui::ConfigTabDiscord *ui;
};

#endif // CONFIG_TAB_DISCORD_H
