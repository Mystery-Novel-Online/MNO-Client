#ifndef CONFIG_TAB_THEME_H
#define CONFIG_TAB_THEME_H

#include <QWidget>

namespace Ui
{
class ConfigTabTheme;
}

class ConfigTabTheme : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigTabTheme(QWidget *parent = nullptr);
  ~ConfigTabTheme();

  void refreshThemeList();
  void refreshGamemodeList();
  void refreshTimeofDayList();
  void updateControls();

  void triggerReload();

signals:
  void reloadTheme();

private slots:
  void on_theme_switch_button_clicked();

  void on_current_theme_dropdown_currentTextChanged(const QString &arg1);

  void on_manual_gamemode_checkbox_stateChanged(int arg1);

  void on_manual_timeofday_checkbox_stateChanged(int arg1);

  void on_current_gamemode_dropdown_currentTextChanged(const QString &arg1);

  void on_current_time_dropdown_currentTextChanged(const QString &arg1);

  void on_resize_spinbox_valueChanged(double arg1);

private:
  Ui::ConfigTabTheme *ui;
};

#endif // CONFIG_TAB_THEME_H
