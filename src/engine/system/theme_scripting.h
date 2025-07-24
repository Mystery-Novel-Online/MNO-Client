#ifndef THEME_SCRIPTING_H
#define THEME_SCRIPTING_H

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


namespace ThemeScripting
{
  void InitializeLua(QString themePath);
};

namespace LuaBridge
{
  sol::function& GetFunction(const char* functionName);

  template<typename... Args>
  inline bool LuaEventCall(const char* functionName, Args&&... args)
  {
    sol::function function = GetFunction(functionName);
    if(!function.valid()) return false;
    function(std::forward<Args>(args)...);
    return true;
  }

  bool OnTabChange(std::string name, std::string group);
  bool OnCharacterMessage(std::string character, std::string folder, std::string emote, std::string message, bool isBlankpost);
  bool SongChangeEvent(std::string path, std::string name, std::string submitter);
  bool OnSongChange(std::string path, std::string name, std::string submitter);
}

namespace LuaFunctions
{
  void ChangeTab(const char* group, const char* tabName);
  void CustomChoiceDialog(const char* text, const char* event);
  void SetNotificationText(const char* text, bool show);
  void AlertUser(bool playSound);
  void SwitchCharacter(const char* characterFolder);
}

#endif // THEME_SCRIPTING_H
