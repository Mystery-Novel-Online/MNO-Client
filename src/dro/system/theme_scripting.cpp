#include "theme_scripting.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/rp_audio.h"

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

static sol::state s_themeScript;

namespace ThemeScripting
{

  void InitializeLua(QString themePath)
  {
    s_themeScript.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math);
    QString filePath = FS::Paths::FindFile("exampleScript.lua");
    if(FS::Checks::FileExists(filePath))
    {
      s_themeScript["play_sfx"] = &LuaFunctions::PlaySfx;
      s_themeScript.safe_script_file(filePath.toStdString());
    }
  }
}

void LuaFunctions::PlaySfx(const char* effectName)
{
  RPAudio::PlayEffect(effectName);
}

bool LuaBridge::CustomCommand(QString commandName)
{
  if(commandName.isEmpty()) return false;
  QString CommandFunction = "onCommand_" + commandName.toLower();
  sol::function customCommandFunction = s_themeScript[CommandFunction.toStdString()];
  if (!customCommandFunction.valid()) return false;
  customCommandFunction();
  return true;
}
