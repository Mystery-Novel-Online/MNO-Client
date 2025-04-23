#include "theme_scripting.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/rp_audio.h"
#include "dro/interface/courtroom_layout.h"
#include <modules/theme/thememanager.h>
#include "modules/managers/notify_manager.h"
#include "courtroom.h"
#include <lobby.h>

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>
#pragma comment(lib, "lua54.lib")

static sol::state s_themeScript;

sol::function s_luaOnTabChange;
sol::function s_luaOnCharacterMessage;

namespace ThemeScripting
{

  void InitializeLua(QString themePath)
  {
    s_themeScript.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::string);
    QString filePath = themePath + "/script.lua";
    if(FS::Checks::FileExists(filePath))
    {
      s_themeScript["play_sfx"] = &LuaFunctions::PlaySfx;
      s_themeScript["change_tab"] = &LuaFunctions::ChangeTab;
      s_themeScript["move_widget"] = &Layout::Courtroom::MoveWidget;
      s_themeScript["create_sticker"] = &Layout::Courtroom::CreateSticker;
      s_themeScript["alert"] = &LuaFunctions::AlertUser;
      s_themeScript["toggle_widget_visibility"] = &Layout::Courtroom::ToggleWidgetVisibility;
      s_themeScript["set_popup_text"] = &LuaFunctions::SetNotificationText;
      s_themeScript.safe_script_file(filePath.toStdString());
      s_luaOnTabChange = s_themeScript["onTabChanged"];
      s_luaOnCharacterMessage = s_themeScript["onCharacterMessage"];
    }
  }
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

bool LuaBridge::onTabChange(QString name, QString group)
{
  if(!s_luaOnTabChange.valid()) return false;
  s_luaOnTabChange(name.toStdString(), group.toStdString());
  return true;
}


bool LuaBridge::QuickCall(const char *eventName)
{
  sol::function eventCall = s_themeScript[eventName];
  if (!eventCall.valid()) return false;
  eventCall();
  return true;
}

bool LuaBridge::QuickCall(const char *eventName, const char *argument)
{
  sol::function eventCall = s_themeScript[eventName];
  if (!eventCall.valid()) return false;
  eventCall(argument);
  return true;
}

bool LuaBridge::OnCharacterMessage(QString character, QString folder, QString emote, QString message)
{
  if(!s_luaOnCharacterMessage.valid()) return false;
  s_luaOnCharacterMessage(character.toStdString(), folder.toStdString(), emote.toStdString(), message.toStdString());
  return true;
}
namespace LuaFunctions
{
  void PlaySfx(const char* effectName)
  {
    RPAudio::PlayEffect(effectName);
  }

  void ChangeTab(const char *group, const char *tabName)
  {
    ThemeManager::get().toggleTab(tabName, group);
  }

  void AlertUser(bool playSound)
  {
    //m_system_player->play(ao_app->get_sfx("word_call"));
    Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
    Lobby *lobby = AOApplication::getInstance()->get_lobby();
    if(courtroom != nullptr)
    {
      AOApplication::getInstance()->alert(courtroom);
    }
    else if(lobby != nullptr)
    {
      AOApplication::getInstance()->alert(lobby);
    }

  }

  void SetNotificationText(const char *text, bool show)
  {
    NotifyManager::get().SetText(text, show);
  }

}
