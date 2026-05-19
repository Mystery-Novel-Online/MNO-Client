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

  QString getBlipSound(const QString& gender);
  bool useBlanks();
  int blipRate();

  void setCharacterBlip(const std::string& set);
  void setTagBlip(const std::string& set);
  void setThemeBlip(const std::string& set);

  void playStartingSfx();
  void playEndingSfx();

private slots:
  void on_blipSet_currentIndexChanged(int index);
  void on_overrideRuleCharacter_stateChanged(int arg1);
  void on_overrideRuleTheme_stateChanged(int arg1);

  void on_blipOverride_toggled(bool arg1);

  void on_blipRate_valueChanged(int arg1);

  void on_blipBlanks_stateChanged(int arg1);

private:
  Ui::ConfigTabBlips *ui;
  std::optional<BlipConfig> m_currentBlip;
  std::optional<BlipConfig> m_characterBlip;
  std::optional<BlipConfig> m_tagBlip;
  std::optional<BlipConfig> m_themeBlip;

  std::optional<std::reference_wrapper<const BlipConfig>> activeBlip() const;

  bool m_allowCharacters = true;
  bool m_allowTheme = true;

  bool m_useOverrides = false;

  int m_blipRate = 0;
  bool m_useBlanks = false;
};

#endif // CONFIG_TAB_BLIPS_H
