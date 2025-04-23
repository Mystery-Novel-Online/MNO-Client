#ifndef THEME_SCRIPTING_H
#define THEME_SCRIPTING_H

#include <QString>


namespace ThemeScripting
{
  void InitializeLua(QString themePath);
};

namespace LuaBridge
{
  bool QuickCall(const char *eventName);
  bool QuickCall(const char *eventName, const char *argument);
  bool CustomCommand(QString commandName);
  bool onTabChange(QString name, QString group);
  bool OnCharacterMessage(QString character, QString folder, QString emote, QString message);
}

namespace LuaFunctions
{
  void PlaySfx(const char* effectName);
  void ChangeTab(const char* group, const char* tabName);
  void SetNotificationText(const char* text, bool show);
  void AlertUser(bool playSound);
}

#endif // THEME_SCRIPTING_H
