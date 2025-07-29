#include "config_tab_theme.h"
#include "ui_config_tab_theme.h"
#include <rolechat/filesystem/RCDir.h>
#include <rolechat/config/ConfigUserSettings.h>
#include "engine/system/config_manager.h"
#include "modules/theme/thememanager.h"

ConfigTabTheme::ConfigTabTheme(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabTheme)
{
  ui->setupUi(this);
  ui->resize_spinbox->setValue(config::ConfigUserSettings::floatValue("resize"));
  ui->manual_gamemode_checkbox->setChecked(config::ConfigUserSettings::booleanValue("manual_gamemode"));
  ui->manual_timeofday_checkbox->setChecked(config::ConfigUserSettings::booleanValue("manual_timeofday"));
  refreshThemeList();
  refreshGamemodeList();
}

ConfigTabTheme::~ConfigTabTheme()
{
  delete ui;
}

void ConfigTabTheme::refreshThemeList()
{
  const QString qCurrentTheme = QString::fromStdString(config::ConfigUserSettings::stringValue("theme"));

  ui->current_theme_dropdown->clear();
  std::optional<int> l_theme_index;

  for (std::string &themeName : fs::RCDir("themes").subDirectories())
  {
    const QString qThemeName = QString::fromStdString(themeName);

    if (qThemeName == qCurrentTheme)
      l_theme_index = ui->current_theme_dropdown->count();

    ui->current_theme_dropdown->addItem(qThemeName);
  }

  if (l_theme_index.has_value())
  {
    ui->current_theme_dropdown->setCurrentIndex(*l_theme_index);
  }

  updateControls();
}

void ConfigTabTheme::refreshGamemodeList()
{
  ui->current_gamemode_dropdown->blockSignals(true);
  bool manualGamemode = ui->manual_gamemode_checkbox->isChecked();
  ui->current_gamemode_dropdown->setEnabled(manualGamemode);

  const QString qCurrentGamemode = QString::fromStdString(config::ConfigUserSettings::stringValue("gamemode"));

  ui->current_gamemode_dropdown->clear();
  ui->current_gamemode_dropdown->addItem("<default>");

  if(manualGamemode)
  {
    std::optional<int> optionalGamemodeIndex;
    QString qGamemodePath = "themes/" + QString::fromStdString(config::ConfigUserSettings::stringValue("theme", "default")) + "/gamemodes/";
    fs::RCDir dirGamemodes = fs::RCDir(qGamemodePath.toStdString());


    for (const std::string &folder : dirGamemodes.subDirectories())
    {
      QString qFolder = QString::fromStdString(folder);
      if (qFolder == "." || qFolder == "..")
        continue;
      if(qFolder == qCurrentGamemode)
        optionalGamemodeIndex = ui->current_gamemode_dropdown->count();

      ui->current_gamemode_dropdown->addItem(qFolder, qFolder);
    }


    if (optionalGamemodeIndex.has_value()){}
      ui->current_gamemode_dropdown->setCurrentIndex(*optionalGamemodeIndex);
  }
  refreshTimeofDayList();
  ui->current_gamemode_dropdown->blockSignals(false);
}

void ConfigTabTheme::refreshTimeofDayList()
{
  ui->current_time_dropdown->blockSignals(true);
  const QString qConfigTimeofday = QString::fromStdString(config::ConfigUserSettings::stringValue("timeofday"));
  bool manualTime = ui->manual_timeofday_checkbox->isChecked();
  ui->current_time_dropdown->setEnabled(manualTime);


  ui->current_time_dropdown->clear();
  ui->current_time_dropdown->addItem("<default>");

  if(manualTime)
  {
    std::optional<int> optionalGamemodeIndex;
    const QString qCurrentTheme = QString::fromStdString(config::ConfigUserSettings::stringValue("theme", "default"));
    const QString qOverrideGamemode = QString::fromStdString(config::ConfigUserSettings::stringValue("gamemode"));

    const QString qCurrentGamemode = manualTime ? qOverrideGamemode : system::ConfigManager::defaultGamemode();

    QString qTimeOfDayPath = qCurrentGamemode.isEmpty() ? "themes/" + qCurrentTheme + "/times/" : "themes/" + qCurrentTheme + "/gamemodes/" + qCurrentGamemode + "/times/";
    fs::RCDir dirTime = fs::RCDir(qTimeOfDayPath.toStdString());

    for (const std::string &folder : dirTime.subDirectories())
    {
      QString qFolder = QString::fromStdString(folder);
      if (qFolder == "." || qFolder == "..")
        continue;
      if(qFolder == qConfigTimeofday)
        optionalGamemodeIndex = ui->current_time_dropdown->count();

      ui->current_time_dropdown->addItem(qFolder, qFolder);
    }

    if (optionalGamemodeIndex.has_value())
      ui->current_time_dropdown->setCurrentIndex(*optionalGamemodeIndex);
  }
  ui->current_time_dropdown->blockSignals(false);
}

void ConfigTabTheme::updateControls()
{
  const QString qCurrentTheme = QString::fromStdString(config::ConfigUserSettings::stringValue("theme"));
  if(ui->current_theme_dropdown->currentText() == qCurrentTheme)
  {
    ui->theme_switch_button->setText("Reload");
  }
  else
  {
    ui->theme_switch_button->setText("Switch");
  }
}

void ConfigTabTheme::triggerReload()
{
  emit reloadTheme();
}

void ConfigTabTheme::on_theme_switch_button_clicked()
{
  ThemeManager::get().toggleReload();
  config::ConfigUserSettings::setString("theme", ui->current_theme_dropdown->currentText().toStdString());
  updateControls();

  emit reloadTheme();
}


void ConfigTabTheme::on_current_theme_dropdown_currentTextChanged(const QString &arg1)
{
  updateControls();
}


void ConfigTabTheme::on_manual_gamemode_checkbox_stateChanged(int arg1)
{
  config::ConfigUserSettings::setValue("manual_gamemode", ui->manual_gamemode_checkbox->isChecked());
  refreshGamemodeList();
  triggerReload();
}


void ConfigTabTheme::on_manual_timeofday_checkbox_stateChanged(int arg1)
{
  config::ConfigUserSettings::setValue("manual_timeofday", ui->manual_timeofday_checkbox->isChecked());
  refreshTimeofDayList();
  triggerReload();
}


void ConfigTabTheme::on_current_gamemode_dropdown_currentTextChanged(const QString &arg1)
{
  QString qProcessedArgument = arg1 == "<default>" ? "" : arg1;
  const QString qConfigValue = QString::fromStdString(config::ConfigUserSettings::stringValue("gamemode"));

  if(qProcessedArgument == qConfigValue)
    return;

  config::ConfigUserSettings::setString("gamemode", qProcessedArgument.toStdString());
  triggerReload();
}


void ConfigTabTheme::on_current_time_dropdown_currentTextChanged(const QString &arg1)
{
  QString qProcessedArgument = arg1 == "<default>" ? "" : arg1;
  const QString qConfigValue = QString::fromStdString(config::ConfigUserSettings::stringValue("timeofday"));

  if(qProcessedArgument == qConfigValue)
    return;

  config::ConfigUserSettings::setString("timeofday", qProcessedArgument.toStdString());
  triggerReload();
}


void ConfigTabTheme::on_resize_spinbox_valueChanged(double arg1)
{
  config::ConfigUserSettings::setValue("resize", (float)arg1);
  ThemeManager::get().setResize(arg1);
}

