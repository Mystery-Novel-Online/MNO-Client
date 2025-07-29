#include "config_manager.h"
#include "config_tabs/config_tab_theme.h"
#include "rolechat_config.h"
#include "modules/theme/thememanager.h"
#include "engine/system/replay_playback.h"

RolechatConfig *system::ConfigManager::m_configWindow = nullptr;
QString system::ConfigManager::m_defaultGamemode = "";
QString system::ConfigManager::m_defaultTimeOfDay = "";

void system::ConfigManager::initializeConfig()
{
  m_configWindow = new RolechatConfig();
}

void system::ConfigManager::showConfig()
{
  m_configWindow->show();
}

void system::ConfigManager::hideConfig()
{
  m_configWindow->hide();
}

QString system::ConfigManager::gamemode()
{
  return config::ConfigUserSettings::booleanValue("manual_gamemode", false) ? QString::fromStdString(config::ConfigUserSettings::stringValue("gamemode")) : defaultGamemode();
}

void system::ConfigManager::setDefaultGamemode(const QString &defaultGamemode)
{
  engine::system::replays::recording::gamemodeChange(defaultGamemode);
  if(m_defaultGamemode == defaultGamemode) return;
  m_defaultGamemode = defaultGamemode;
  ThemeManager::get().LoadGamemode(defaultGamemode);

  ConfigTabTheme* themeConfig = retrieveTab<ConfigTabTheme>("Theme");
  if(!themeConfig)
    return;
  themeConfig->triggerReload();
}

QString system::ConfigManager::timeOfDay()
{
  return config::ConfigUserSettings::booleanValue("manual_timeofday", false) ? QString::fromStdString(config::ConfigUserSettings::stringValue("timeofday")) : defaultTimeOfDay();
}

void system::ConfigManager::setdefaultTimeOfDay(const QString &defaultTime)
{
  engine::system::replays::recording::todChange(defaultTime);
  if(m_defaultTimeOfDay == defaultTime) return;
  m_defaultTimeOfDay = defaultTime;

  ConfigTabTheme* themeConfig = retrieveTab<ConfigTabTheme>("Theme");
  if(!themeConfig)
    return;
  themeConfig->triggerReload();
}
