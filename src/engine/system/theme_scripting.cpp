#include "theme_scripting.h"
#include "engine/fs/fs_reading.h"
#include "engine/system/audio.h"
#include "engine/interface/courtroom_layout.h"
#include <modules/theme/thememanager.h>
#include "modules/managers/notify_manager.h"
#include "engine/network/metadata/area_metadata.h"
#include "engine/system/runtime_values.h"
#include "scripting/lua/LuaSyncedVariable.h"

#include <lobby.h>

#pragma comment(lib, "lua54.lib")

static LuaRuntime s_theme;
static LuaRuntime s_minigame;

namespace ThemeScripting
{

void InitializeLua(QString themePath, LuaTarget target)
  {

    auto& runtime = target == LuaTarget::Minigame ? s_minigame : s_theme;
    sol::state& targetScript = runtime.state;

    audio::blip::setBlipRate(-1);
    runtime.functionCache.clear();
    targetScript = sol::state();
    targetScript.open_libraries(sol::lib::base, sol::lib::io, sol::lib::math, sol::lib::string, sol::lib::table);

    targetScript.new_usertype<LuaSyncedVariable>("SyncedVariable",
        sol::meta_function::index, &LuaSyncedVariable::get,
        sol::meta_function::new_index, &LuaSyncedVariable::set
    );

    targetScript.set_function("AreaVariable", [](const std::string& key)
    {
      return LuaSyncedVariable(key, SyncedScope::Area);
    });

    targetScript.set_function("HubVariable", [](const std::string& key)
    {
      return LuaSyncedVariable(key, SyncedScope::Hub);
    });

    targetScript.set_function("UserVariable", [](const std::string& key)
    {
      return LuaSyncedVariable(key, SyncedScope::User);
    });

    QString filePath = themePath + "/script.lua";
    if(FS::Checks::FileExists(filePath))
    {

      {
        sol::table audio = targetScript.create_named_table("Audio");

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

      sol::table widgetTable = targetScript.create_named_table("Widget");
      widgetTable.set_function("Move", &courtroom::layout::moveWidget);
      widgetTable.set_function("Resize", &courtroom::layout::resizeWidget);
      widgetTable.set_function("Raise", &courtroom::layout::raiseWidget);
      widgetTable.set_function("SetVisible", &courtroom::layout::setVisibility);
      widgetTable.set_function("SetParent", &courtroom::layout::setParent);

      sol::table dialogTable = targetScript.create_named_table("ChoiceDialog");
      dialogTable.set_function("TriggerCustom", &LuaFunctions::CustomChoiceDialog);
      dialogTable.set_function("SetText", &LuaFunctions::SetNotificationText);

      sol::table characterTable = targetScript.create_named_table("Character");
      characterTable.set_function("GetShowname", &user::getShowname);
      characterTable.set_function("GetIniswap", &user::getIniswap);
      characterTable.set_function("Exists", &FS::Checks::CharacterExists);
      characterTable.set_function("Switch", &LuaFunctions::SwitchCharacter);
      characterTable.set_function("SetScale", &courtroom::sliders::setScale);
      characterTable.set_function("SetVertical", &courtroom::sliders::setVertical);
      characterTable.set_function("SetHorizontal", &courtroom::sliders::setHorizontal);

      sol::table buttonsTable = targetScript.create_named_table("Button");
      buttonsTable.set_function("Create", &courtroom::buttons::create);

      sol::table displayTextTable = targetScript.create_named_table("TextDisplay");
      displayTextTable.set_function("Create", &courtroom::textedit::create);
      displayTextTable.set_function("SetText", &courtroom::textedit::setText);
      displayTextTable.set_function("SetFrame", &courtroom::textedit::setFrame);

      sol::table hoverWidgetTable = targetScript.create_named_table("HoverController");
      hoverWidgetTable.set_function("Create", &courtroom::hovercontroller::create);
      hoverWidgetTable.set_function("AddWidget", &courtroom::hovercontroller::addWidget);

      sol::table lineeditTable = targetScript.create_named_table("LineEdit");
      lineeditTable.set_function("Create", &courtroom::lineedit::create);
      lineeditTable.set_function("GetValue", &courtroom::lineedit::getValue);
      lineeditTable.set_function("SetValue", &courtroom::lineedit::setValue);

      sol::table comboboxTable = targetScript.create_named_table("ComboBox");
      comboboxTable.set_function("Create", &courtroom::combobox::create);
      comboboxTable.set_function("AddItem", &courtroom::combobox::addItem);
      comboboxTable.set_function("Clear", &courtroom::combobox::clearItems);

      sol::table slidersTable = targetScript.create_named_table("Slider");
      slidersTable.set_function("Create", &courtroom::sliders::create);
      slidersTable.set_function("CreateVertical", &courtroom::sliders::createVertical);
      slidersTable.set_function("SetValue", &courtroom::sliders::setValue);
      slidersTable.set_function("GetValue", &courtroom::sliders::getValue);


      sol::table stickerTable = targetScript.create_named_table("Sticker");
      stickerTable.set_function("Create", &courtroom::stickers::create);

      sol::table tabTable = targetScript.create_named_table("Tabs");
      tabTable.set_function("Change", &LuaFunctions::ChangeTab);

      sol::table serverTable = targetScript.create_named_table("Server");
      tabTable.set_function("GetClientId", &user::getClientId);
      tabTable.set_function("GetCharacterId", &user::GetCharacterId);
      tabTable.set_function("GetCurrentCharacter", &user::GetCharacterName);

      {
        sol::table ic = targetScript.create_named_table("IC");
        sol::table inputField = ic.create_named("InputField");

        inputField.set_function("Focus", &courtroom::ic::focusMessageBox);
        inputField.set_function("GetText", &courtroom::ic::getMessageBoxContents);
        inputField.set_function("SetText", &courtroom::ic::setMessageBox);
        inputField.set_function("Append", &courtroom::ic::appendMessageBox);
      }

      {
        sol::table table = targetScript.create_named_table("Viewport");
        table.set_function("Screenshot", &courtroom::viewport::screenshot);
        table.set_function("AddInteraction", &courtroom::viewport::addInteraction);
        table.set_function("ClearInteractions", &courtroom::viewport::clearInteractions);
      }

      {
        sol::table table = targetScript.create_named_table("Areas");
        table.set_function("MoveByName", &courtroom::areas::switchName);
      }

      {
        sol::table table = targetScript.create_named_table("Animator");
        table.set_function("Create", &courtroom::animations::createAnimation);
        table.set_function("AddKeyframe", &courtroom::animations::addKeyframe);
        table.set_function("Reset", &courtroom::animations::reset);
        table.set_function("Start", &courtroom::animations::playAnimation);
        table.set_function("Stop", &courtroom::animations::stopAnimation);
      }

      {
        sol::table table = targetScript.create_named_table("RuntimeValue");
        table.set_function("Resolve", &engine::runtime::values::resolveVariables);
        table.set_function("Assign", &engine::runtime::values::storeValue);
      }

      {
        sol::table ooc = targetScript.create_named_table("OOC");

        sol::table log = ooc.create_named("Log");
        sol::table name = ooc.create_named("Name");
        sol::table inputField = ooc.create_named("InputField");

        log.set_function("Append", &courtroom::ooc::appendMessage);

        name.set_function("Get", &courtroom::ooc::getDisplayName);
        name.set_function("Set", &courtroom::ooc::setDisplayName);

        inputField.set_function("GetText", &courtroom::ooc::getInputFieldContents);
        inputField.set_function("SetText", &courtroom::ooc::setInputFieldContents);
      }

      sol::table systemTable = targetScript.create_named_table("System");
      systemTable.set_function("Alert", &LuaFunctions::AlertUser);

      sol::table areaTable = targetScript.create_named_table("Area");
      areaTable.set_function("SetDescription", &AreaMetadata::SetDescription);

      targetScript.safe_script_file(filePath.toUtf8().constData());
    }
  }
}

namespace LuaBridge
{
  sol::function &GetFunction(const std::string &functionName, LuaTarget target)
  {
    auto& runtime = target == LuaTarget::Minigame ? s_minigame : s_theme;

    if(runtime.functionCache.contains(functionName)) return runtime.functionCache[functionName];
    sol::function eventCall = runtime.state[functionName];
    if(eventCall.valid()) runtime.functionCache[functionName] = eventCall;
    return runtime.functionCache[functionName];
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
  void ChangeTab(const std::string& group, const std::string& tabName)
  {
    ThemeManager::get().toggleTab(QString::fromStdString(tabName), QString::fromStdString(group));
  }

  void AlertUser(bool playSound)
  {
    if(playSound) audio::system::Play(AOApplication::getInstance()->get_sfx("word_call").toStdString());

    Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
    Lobby *lobby = AOApplication::getInstance()->get_lobby();

    if(courtroom != nullptr)
      AOApplication::getInstance()->alert(courtroom);
    else if(lobby != nullptr)
      AOApplication::getInstance()->alert(lobby);
  }

  void SetNotificationText(const std::string& text, bool show)
  {
    NotifyManager::get().SetText(QString::fromStdString(text), show);
  }

  void CustomChoiceDialog(const std::string& text, const std::string& event)
  {
    NotifyManager::get().SetLuaNotification(QString::fromStdString(text), QString::fromStdString(event));
  }

  void SwitchCharacter(const std::string& characterFolder)
  {
    Courtroom *courtroom = AOApplication::getInstance()->get_courtroom();
    if(courtroom == nullptr) return;
    courtroom->SwitchCharacterByName(characterFolder);
  }

}
