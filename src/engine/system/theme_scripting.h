#ifndef THEME_SCRIPTING_H
#define THEME_SCRIPTING_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

struct LuaRuntime
{
  sol::state state;
  QMap<std::string, sol::function> functionCache;
};

enum class LuaTarget
{
  Theme,
  Minigame
};

namespace ThemeScripting
{
  void InitializeLua(QString themePath, LuaTarget target = LuaTarget::Theme);
};

namespace LuaBridge
{
  sol::function& GetFunction(const std::string& functionName, LuaTarget target = LuaTarget::Theme);

  template<typename... Args>
  inline bool LuaEventCall(const std::string& functionName, Args&&... args)
  {
    bool success = false;
    {
      sol::function function = GetFunction(functionName);
      if(function.valid())
      {
        function(std::forward<Args>(args)...);
        success = true;
      }
    }

    {
      sol::function function = GetFunction(functionName, LuaTarget::Minigame);
      if(function.valid())
      {
        function(std::forward<Args>(args)...);
        success = true;
      }
    }
    return success;
  }

  bool OnTabChange(std::string name, std::string group);
  bool OnCharacterMessage(std::string character, std::string folder, std::string emote, std::string message, bool isBlankpost);
  bool SongChangeEvent(std::string path, std::string name, std::string submitter);
  bool OnSongChange(std::string path, std::string name, std::string submitter);
}

namespace LuaFunctions
{
  void ChangeTab(const std::string& group, const std::string& tabName);
  void CustomChoiceDialog(const std::string& text, const std::string& event);
  void SetNotificationText(const std::string& text, bool show);
  void AlertUser(bool playSound);
  void SwitchCharacter(const std::string& characterFolder);
}

#endif // THEME_SCRIPTING_H
