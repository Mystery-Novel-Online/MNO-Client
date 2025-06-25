#include "theme_scripting.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/audio.h"
#include "dro/interface/courtroom_layout.h"
#include <modules/theme/thememanager.h>
#include "modules/managers/notify_manager.h"
#include "dro/network/metadata/area_metadata.h"
#include "courtroom.h"
#include <lobby.h>

#pragma comment(lib, "lua54.lib")

static sol::state s_themeScript;
static QMap<std::string, sol::function> s_registeredFunctions;

namespace ThemeScripting
{

  void InitializeLua(QString themePath)
  {
    audio::blip::setBlipRate(-1);
    s_registeredFunctions.clear();
    s_themeScript = sol::state();
    s_themeScript.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::string, sol::lib::table);
    QString filePath = themePath + "/script.lua";
    if(FS::Checks::FileExists(filePath))
    {

      {
        sol::table audio = s_themeScript.create_named_table("Audio");

        sol::table bgm = audio.create_named("BGM");
        sol::table blip = audio.create_named("Blip");
        sol::table sfx = audio.create_named("SFX");
        sol::table system = audio.create_named("System");
        system.set_function("Play", &audio::system::Play);

        sfx.set_function("Play", &audio::effect::Play);

        bgm.set_function("Play", &audio::bgm::Play);
        bgm.set_function("Stop", &audio::bgm::Stop);
        bgm.set_function("SetSpeed", &audio::bgm::SetSpeed);
        bgm.set_function("SetPitch", &audio::bgm::SetPitch);
        bgm.set_function("ToggleReverb", &audio::bgm::SetReverb);

        blip.set_function("Tick", &audio::blip::Tick);
        blip.set_function("SetRate", &audio::blip::setBlipRate);
        blip.set_function("SetGender", &audio::blip::SetGender);
        blip.set_function("SetSound", &audio::blip::SetSound);
      }

      sol::table widgetTable = s_themeScript.create_named_table("Widget");
      widgetTable.set_function("Move", &courtroom::layout::moveWidget);
      widgetTable.set_function("Raise", &courtroom::layout::raiseWidget);
      widgetTable.set_function("SetVisible", &courtroom::layout::setVisibility);
      widgetTable.set_function("SetParent", &courtroom::layout::setParent);

      sol::table dialogTable = s_themeScript.create_named_table("ChoiceDialog");
      dialogTable.set_function("TriggerCustom", &LuaFunctions::CustomChoiceDialog);
      dialogTable.set_function("SetText", &LuaFunctions::SetNotificationText);

      sol::table characterTable = s_themeScript.create_named_table("Character");
      characterTable.set_function("GetShowname", &metadata::user::getShowname);
      characterTable.set_function("GetIniswap", &metadata::user::getIniswap);
      characterTable.set_function("Exists", &FS::Checks::CharacterExists);
      characterTable.set_function("Switch", &LuaFunctions::SwitchCharacter);
      characterTable.set_function("SetScale", &courtroom::sliders::setScale);
      characterTable.set_function("SetVertical", &courtroom::sliders::setVertical);
      characterTable.set_function("SetHorizontal", &courtroom::sliders::setHorizontal);

      sol::table buttonsTable = s_themeScript.create_named_table("Button");
      buttonsTable.set_function("Create", &courtroom::buttons::create);

      sol::table slidersTable = s_themeScript.create_named_table("Slider");
      slidersTable.set_function("Create", &courtroom::sliders::create);
      slidersTable.set_function("CreateVertical", &courtroom::sliders::createVertical);
      slidersTable.set_function("SetValue", &courtroom::sliders::setValue);
      slidersTable.set_function("GetValue", &courtroom::sliders::getValue);


      sol::table stickerTable = s_themeScript.create_named_table("Sticker");
      stickerTable.set_function("Create", &courtroom::stickers::create);

      sol::table tabTable = s_themeScript.create_named_table("Tabs");
      tabTable.set_function("Change", &LuaFunctions::ChangeTab);

      sol::table serverTable = s_themeScript.create_named_table("Server");
      tabTable.set_function("GetClientId", &metadata::user::getClientId);
      tabTable.set_function("GetCharacterId", &metadata::user::GetCharacterId);
      tabTable.set_function("GetCurrentCharacter", &metadata::user::GetCharacterName);

      {
        sol::table ic = s_themeScript.create_named_table("IC");
        sol::table inputField = ic.create_named("InputField");

        inputField.set_function("Focus", &courtroom::ic::focusMessageBox);
        inputField.set_function("GetText", &courtroom::ic::getMessageBoxContents);
        inputField.set_function("SetText", &courtroom::ic::setMessageBox);
        inputField.set_function("Append", &courtroom::ic::appendMessageBox);
      }

      {
        sol::table ooc = s_themeScript.create_named_table("OOC");

        sol::table log = ooc.create_named("Log");
        sol::table name = ooc.create_named("Name");
        sol::table inputField = ooc.create_named("InputField");

        log.set_function("Append", &courtroom::ooc::appendMessage);

        name.set_function("Get", &courtroom::ooc::getDisplayName);
        name.set_function("Set", &courtroom::ooc::setDisplayName);

        inputField.set_function("GetText", &courtroom::ooc::getInputFieldContents);
        inputField.set_function("SetText", &courtroom::ooc::setInputFieldContents);
      }

      sol::table systemTable = s_themeScript.create_named_table("System");
      systemTable.set_function("Alert", &LuaFunctions::AlertUser);

      sol::table areaTable = s_themeScript.create_named_table("Area");
      areaTable.set_function("SetDescription", &AreaMetadata::SetDescription);

      s_themeScript.safe_script_file(filePath.toUtf8().constData());
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

  bool OnCharacterMessage(std::string character, std::string folder, std::string emote, std::string message, bool isBlankpost)
  {
    return LuaEventCall("OnMessageReceived", character, folder, emote, message, isBlankpost);
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
    if(playSound) audio::system::Play(AOApplication::getInstance()->get_sfx("word_call").toUtf8());

    Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
    Lobby *lobby = AOApplication::getInstance()->get_lobby();

    if(courtroom != nullptr)
      AOApplication::getInstance()->alert(courtroom);
    else if(lobby != nullptr)
      AOApplication::getInstance()->alert(lobby);
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
