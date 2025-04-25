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

static QMap<std::string, sol::function> s_registeredFunctions;

namespace ThemeScripting
{

  void InitializeLua(QString themePath)
  {
    s_registeredFunctions.clear();
    s_themeScript = sol::state();
    s_themeScript.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::string);
    QString filePath = themePath + "/script.lua";
    if(FS::Checks::FileExists(filePath))
    {

      sol::table audioTable = s_themeScript.create_named_table("Audio");

      sol::table systemAudioTable = s_themeScript.create_table();
      systemAudioTable.set_function("Play", &RPAudio::PlaySystem);

      sol::table sfxTable = s_themeScript.create_table();
      sfxTable.set_function("Play", &RPAudio::PlayEffect);

      sol::table bgmTable = s_themeScript.create_table();
      bgmTable.set_function("Play", &RPAudio::PlayBGM);
      bgmTable.set_function("SetSpeed", &RPAudio::SetBGMSpeed);
      bgmTable.set_function("SetPitch", &RPAudio::SetBGMPitch);
      bgmTable.set_function("ToggleReverb", &RPAudio::SetBGMReverb);

      sol::table blipTable = s_themeScript.create_table();
      blipTable.set_function("Tick", &RPAudio::BlipTick);
      blipTable.set_function("SetGender", &RPAudio::SetBlipGender);
      blipTable.set_function("SetSound", &RPAudio::SetBlipSound);

      audioTable.set("BGM", bgmTable);
      audioTable.set("Blip", blipTable);
      audioTable.set("SFX", sfxTable);
      audioTable.set("System", systemAudioTable);


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
      characterTable.set_function("SetScaleOffset", &Layout::Courtroom::SetScaleSlider);
      characterTable.set_function("SetVerticalOffset", &Layout::Courtroom::SetVerticalSlider);

      sol::table stickerTable = s_themeScript.create_named_table("Sticker");
      stickerTable.set_function("Create", &Layout::Courtroom::CreateSticker);

      sol::table tabTable = s_themeScript.create_named_table("Tabs");
      tabTable.set_function("Change", &LuaFunctions::ChangeTab);

      sol::table chatlogTable = s_themeScript.create_named_table("Chatlog");
      chatlogTable.set_function("AppendOOC", &Layout::Courtroom::AppendToOOC);

      sol::table systemTable = s_themeScript.create_named_table("System");
      systemTable.set_function("Alert", &LuaFunctions::AlertUser);

      s_themeScript.safe_script_file(filePath.toStdString());
    }
  }
}

namespace LuaBridge
{

  sol::function &GetFunction(const char* functionName)
  {
    if(s_registeredFunctions.contains(functionName)) return s_registeredFunctions[functionName];
    sol::function eventCall = s_themeScript[functionName];
    if(eventCall.valid()) s_registeredFunctions[functionName] = eventCall;
    return s_registeredFunctions[functionName];
  }

  bool OnTabChange(std::string name, std::string group)
  {
    return LuaEventCall("OnTabChanged", name, group);
  }

  bool OnCharacterMessage(std::string character, std::string folder, std::string emote, std::string message)
  {
    return LuaEventCall("OnCharacterMessage", character, folder, emote, message);
  }

  bool SongChangeEvent(std::string path, std::string name, std::string submitter)
  {
    return LuaEventCall("SongChangeEvent", path, name, submitter);
  }

  bool OnSongChange(std::string path, std::string name, std::string submitter)
  {
    return LuaEventCall("OnSongChange", path, name, submitter);
  }

}

namespace LuaFunctions
{
  void ChangeTab(const char *group, const char *tabName)
  {
    ThemeManager::get().toggleTab(tabName, group);
  }

  void AlertUser(bool playSound)
  {
    if(playSound) RPAudio::PlaySystem(AOApplication::getInstance()->get_sfx("word_call").toUtf8());
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
