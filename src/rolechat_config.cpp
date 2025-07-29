#include "rolechat_config.h"
#include "ui_rolechat_config.h"
#include "config_tabs/config_tab_theme.h"
#include <rolechat/config/ConfigUserSettings.h>

RolechatConfig::RolechatConfig(QWidget *parent) : QMainWindow(parent), ui(new Ui::RolechatConfig)
{
  ui->setupUi(this);
  config::ConfigUserSettings::load();
  setupTabs();
}

RolechatConfig::~RolechatConfig()
{
  delete ui;
}

QWidget *RolechatConfig::findTab(const QString &name)
{
  if(m_tabWidgets.contains(name))
    return m_tabWidgets[name];
  return nullptr;
}

void RolechatConfig::setupTabs()
{
  m_tabWidgets.clear();
  m_tabWidgets["Theme"] = new ConfigTabTheme();
  ui->category_list_widget->addItems(m_tabCategories.keys());
}

void RolechatConfig::switchCategory(QString category)
{
  ui->configuration_tab_handler->clear();
  for(const QString &tabName : m_tabCategories[category])
    ui->configuration_tab_handler->addTab(m_tabWidgets[tabName], tabName);
}

void RolechatConfig::on_config_save_button_clicked()
{
  config::ConfigUserSettings::save();
}


void RolechatConfig::on_category_list_widget_currentTextChanged(const QString &currentText)
{
  switchCategory(currentText);
}

