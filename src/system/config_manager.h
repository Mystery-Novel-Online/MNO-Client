#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H
#include "rolechat_config.h"
namespace engine::system
{
class ConfigManager
{
public:
  static void initializeConfig();
  static void showConfig();
  static void hideConfig();

  static QString gamemode();
  static QString defaultGamemode() {return m_defaultGamemode; }
  static void setDefaultGamemode(const QString& defaultGamemode);

  static QString timeOfDay();
  static QString defaultTimeOfDay() {return m_defaultTimeOfDay; }
  static void setdefaultTimeOfDay(const QString& defaultTime);

  template <typename T>
  static T* retrieveTab(const QString& tabName)
  {
    QWidget* widget = m_configWindow->findTab(tabName);
    if(widget == nullptr) return nullptr;

    T *tabWidget = dynamic_cast<T *>(widget);

    return tabWidget;
  }


private:
  static RolechatConfig *m_configWindow;
  static QString m_defaultGamemode;
  static QString m_defaultTimeOfDay;
};

}
#endif // CONFIG_MANAGER_H
