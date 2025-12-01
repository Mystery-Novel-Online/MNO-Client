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
  sol::function& GetFunction(const std::string& functionName);

  template<typename... Args>
  inline bool LuaEventCall(const std::string& functionName, Args&&... args)
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
  void ChangeTab(const std::string& group, const std::string& tabName);
  void CustomChoiceDialog(const std::string& text, const std::string& event);
  void SetNotificationText(const std::string& text, bool show);
  void AlertUser(bool playSound);
  void SwitchCharacter(const std::string& characterFolder);
}

#endif // THEME_SCRIPTING_H
