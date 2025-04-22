#ifndef THEME_SCRIPTING_H
#define THEME_SCRIPTING_H

#include <QString>


namespace ThemeScripting
{
  void InitializeLua(QString themePath);
};

namespace LuaBridge
{
  bool QuickCall(QString eventName);
  bool QuickCall(QString eventName, QString argument);
  bool CustomCommand(QString commandName);
  bool onTabChange(QString name, QString group);
  bool OnCharacterMessage(QString character, QString folder, QString emote, QString message);
}

namespace LuaFunctions
{
  void PlaySfx(const char* effectName);
  void ChangeTab(const char* group, const char* tabName);
}

#endif // THEME_SCRIPTING_H
