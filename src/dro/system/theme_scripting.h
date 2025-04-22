#ifndef THEME_SCRIPTING_H
#define THEME_SCRIPTING_H

#include <QString>


namespace ThemeScripting
{
  void InitializeLua(QString themePath);
};

namespace LuaBridge
{
  bool CustomCommand(QString commandName);
}

namespace LuaFunctions
{
  void PlaySfx(const char* effectName);
}

#endif // THEME_SCRIPTING_H
