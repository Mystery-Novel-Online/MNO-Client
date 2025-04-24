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

      sol::table audioTable = s_themeScript.create_named_table("Audio");
      audioTable.set_function("PlaySFX", &LuaFunctions::PlaySfx);

      sol::table widgetTable = s_themeScript.create_named_table("Widget");
      widgetTable.set_function("Move", &Layout::Courtroom::MoveWidget);
      widgetTable.set_function("Raise", &Layout::Courtroom::RaiseWidget);
      widgetTable.set_function("SetVisible", &Layout::Courtroom::ToggleWidgetVisibility);

      sol::table dialogTable = s_themeScript.create_named_table("ChoiceDialog");
      dialogTable.set_function("TriggerCustom", &LuaFunctions::CustomChoiceDialog);
      dialogTable.set_function("SetText", &LuaFunctions::SetNotificationText);

      sol::table characterTable = s_themeScript.create_named_table("Character");
      characterTable.set_function("Exists", &FS::Checks::CharacterExists);
      characterTable.set_function("Switch", &LuaFunctions::SwitchCharacter);

      sol::table stickerTable = s_themeScript.create_named_table("Sticker");
      stickerTable.set_function("Create", &Layout::Courtroom::CreateSticker);

      sol::table tabTable = s_themeScript.create_named_table("Tabs");
      tabTable.set_function("Change", &LuaFunctions::ChangeTab);

      s_themeScript["alert"] = &LuaFunctions::AlertUser;

      s_themeScript.safe_script_file(filePath.toStdString());

      s_luaOnTabChange = s_themeScript["OnTabChanged"];
      s_luaOnCharacterMessage = s_themeScript["OnCharacterMessage"];
    }
  }
}

bool LuaBridge::CustomCommand(QString commandName)
{
  if(commandName.isEmpty()) return false;
  QString CommandFunction = "OnCommand_" + commandName.toLower();
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

  void CustomChoiceDialog(const char *text, const char *event)
  {
    NotifyManager::get().SetLuaNotification(text, event);
  }

  void SwitchCharacter(const char *characterFolder)
  {
    Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
    if(courtroom == nullptr) return;
    courtroom->SwitchCharacterByName(characterFolder);
  }

}
