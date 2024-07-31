#include "aoconfig.h"

#include "commondefs.h"
#include "datatypes.h"
#include "draudioengine.h"
#include "drpather.h"
#include "mk2/spritedynamicreader.h"
#include "modules/managers/scene_manager.h"
#include "modules/managers/localization_manager.h"

// qt
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMap>
#include <QSettings>
#include <QSharedPointer>
#include <QVector>

#include <modules/theme/thememanager.h>

#include <modules/managers/replay_manager.h>
#include <modules/managers/variable_manager.h>

/*!
    We have to suffer through a lot of boilerplate code
    but hey, when has ao2 ever cared?
    Wait, am I using the term wrong? Nice.
*/
class AOConfigPrivate : public QObject
{
  Q_OBJECT

public:
  AOConfigPrivate();
  ~AOConfigPrivate();

  // setters
public slots:
  void load_file();
  void save_file();

private:
  void invoke_signal(QString p_method_name, QGenericArgument p_arg1 = QGenericArgument(nullptr), QGenericArgument p_arg2 = QGenericArgument());
  void update_favorite_device();

private slots:
  void on_application_state_changed(Qt::ApplicationState p_state);

private:
  friend class AOConfig;

  QSettings cfg;
  // hate me more
  QVector<QObject *> children;

  // data
  bool first_launch;
  bool autosave;
  QStringList notification_filter;
  QString username;
  QString callwords;
  QString server_advertiser;
  bool server_alerts;
  bool discord_presence = false;
  bool discord_hide_server = false;
  bool discord_hide_character = false;
  QString language;
  QString theme;
  QString gamemode;
  QString manual_gamemode;
  bool manual_gamemode_selection;
  QString timeofday;
  QString manual_timeofday;
  bool manual_timeofday_selection;
  QString showname;
  QString showname_placeholder;
  QMap<QString, QString> ini_map;
  bool searchable_iniswap;
  bool always_pre;
  int chat_tick_interval;
  bool emote_preview;
  bool sticky_sfx;
  int message_length_threshold;
  int log_max_lines;
  bool log_display_timestamp;
  bool log_display_client_id;
  bool log_display_self_highlight;
  bool log_display_empty_messages;
  bool log_is_topdown;
  bool log_format_use_newline;
  bool log_display_music_switch;
  bool log_is_recording;

  // performance
  int system_memory_threshold;
  QMap<int, bool> sprite_caching;
  int loading_bar_delay;
  int caching_threshold;

  // audio
  std::optional<QString> favorite_device_driver;
  int master_volume;
  bool suppress_background_audio;
  int system_volume;
  int effect_volume;
  bool effect_ignore_suppression;
  int music_volume;
  bool music_ignore_suppression;
  int video_volume;
  bool video_ignore_suppression;
  int blip_volume;
  bool blip_ignore_suppression;
  int blip_rate;
  int punctuation_delay;
  double theme_resize;
  int fade_duration;
  bool blank_blips;

  // audio sync
  DRAudioEngine *audio_engine = nullptr;

  // shortcuts
  QKeySequence screenshot_shortcut;
  QKeySequence look_shortcut;
};

AOConfigPrivate::AOConfigPrivate()
    : QObject(nullptr)
    , cfg(DRPather::get_application_path() + BASE_CONFIG_INI, QSettings::IniFormat)
    , audio_engine(new DRAudioEngine(this))
{
  Q_ASSERT_X(qApp, "initialization", "QGuiApplication is required");
  connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)), this, SLOT(on_application_state_changed(Qt::ApplicationState)));

  load_file();
}

AOConfigPrivate::~AOConfigPrivate()
{}

void AOConfigPrivate::load_file()
{
  first_launch = cfg.value("first_launch", true).toBool();

  if (first_launch)
  {
    cfg.setValue("first_launch", false);
    cfg.sync();
  }

  autosave = cfg.value("autosave", true).toBool();

  { // notifications
    notification_filter.clear();
    cfg.beginGroup("notifications");
    for (const QString &i_key : cfg.childKeys())
    {
      const QString l_message = cfg.value(i_key).toString();
      if (l_message.isEmpty())
        continue;
      notification_filter.append(l_message);
    }
    cfg.endGroup();
  }

  username = cfg.value("username").toString();
  showname = cfg.value("showname").toString();
  callwords = cfg.value("callwords").toString();
  server_advertiser = cfg.value("server_advertiser", "https://servers.aceattorneyonline.com").toString();
  server_alerts = cfg.value("server_alerts", true).toBool();

  discord_presence = cfg.value("discord_presence", true).toBool();
  discord_hide_server = cfg.value("discord_hide_server", false).toBool();
  discord_hide_character = cfg.value("discord_hide_character", false).toBool();

  language = cfg.value("language").toString();
  if (language.trimmed().isEmpty())
    language = "English";
  LocalizationManager::get().setLanguage(language);

  theme = cfg.value("theme").toString();
  if (theme.trimmed().isEmpty())
    theme = "default";

  manual_gamemode = cfg.value("gamemode").toString();
  manual_gamemode_selection = cfg.value("manual_gamemode", false).toBool();
  manual_timeofday = cfg.value("timeofday").toString();
  manual_timeofday_selection = cfg.value("manual_timeofday", false).toBool();
  searchable_iniswap = cfg.value("searchable_iniswap", true).toBool();
  always_pre = cfg.value("always_pre", true).toBool();
  chat_tick_interval = cfg.value("chat_tick_interval", 60).toInt();
  emote_preview = cfg.value("emote_preview", true).toBool();
  sticky_sfx = cfg.value("sticky_sfx", false).toBool();
  message_length_threshold = cfg.value("message_length_threshold", 70).toInt();
  log_max_lines = cfg.value("chatlog_limit", 100).toInt();
  log_is_topdown = cfg.value("chatlog_scrolldown", true).toBool();
  log_display_timestamp = cfg.value("chatlog_display_timestamp", true).toBool();
  log_display_client_id = cfg.value("chatlog_display_client_id", false).toBool();
  log_display_self_highlight = cfg.value("chatlog_display_self_highlight", true).toBool();
  log_display_empty_messages = cfg.value("chatlog_display_empty_messages", false).toBool();
  log_format_use_newline = cfg.value("chatlog_newline", false).toBool();
  log_display_music_switch = cfg.value("music_change_log", true).toBool();
  log_is_recording = cfg.value("enable_logging", true).toBool();

  // performance
  {
    sprite_caching.clear();
    cfg.beginGroup("sprite_caching");
    const QStringList l_key_list = sprite_category_string_list();
    for (const QString &i_key : l_key_list)
    {
      const SpriteCategory l_category = string_to_sprite_category(i_key);
      sprite_caching.insert(l_category, cfg.value(i_key, true).toBool());
    }
    cfg.endGroup();
  }
  system_memory_threshold = qBound(10, cfg.value("system_memory_threshold", 50).toInt(), 80);
  loading_bar_delay = qBound(0, cfg.value("loading_bar_delay", 500).toInt(), 2000);
  caching_threshold = qBound(0, cfg.value("caching_threshold", 50).toInt(), 100);
  mk2::SpriteDynamicReader::set_system_memory_threshold(system_memory_threshold);

  // audio
  if (cfg.contains("favorite_device_driver"))
    favorite_device_driver = cfg.value("favorite_device_driver").toString();

  suppress_background_audio = cfg.value("suppress_background_audio").toBool();
  master_volume = cfg.value("default_master", 50).toInt();
  system_volume = cfg.value("default_system", 50).toInt();
  effect_volume = cfg.value("default_sfx", 50).toInt();
  effect_ignore_suppression = cfg.value("effect_ignore_suppression", false).toBool();
  music_volume = cfg.value("default_music", 50).toInt();
  music_ignore_suppression = cfg.value("music_ignore_suppression", false).toBool();
  video_volume = cfg.value("default_video", 50).toInt();
  video_ignore_suppression = cfg.value("video_ignore_suppression", false).toBool();
  blip_volume = cfg.value("default_blip", 50).toInt();
  blip_ignore_suppression = cfg.value("blip_ignore_suppression", false).toBool();
  blip_rate = cfg.value("blip_rate", 1000000000).toInt();
  punctuation_delay = cfg.value("punctuation_delay", 110).toInt();
  theme_resize = cfg.value("theme_resize", 1).toDouble();
  ThemeManager::get().SetResizeClient(theme_resize);
  fade_duration = cfg.value("fade_duration", 200).toInt();
  SceneManager::get().setFadeDuration(fade_duration);
  blank_blips = cfg.value("blank_blips").toBool();

  // audio update
  audio_engine->set_volume(master_volume);
  audio_engine->get_family(DRAudio::Family::FSystem)->set_volume(system_volume);
  audio_engine->get_family(DRAudio::Family::FEffect)->set_volume(effect_volume);
  audio_engine->get_family(DRAudio::Family::FEffect)->set_ignore_suppression(effect_ignore_suppression);
  audio_engine->get_family(DRAudio::Family::FMusic)->set_volume(music_volume);
  audio_engine->get_family(DRAudio::Family::FMusic)->set_ignore_suppression(effect_ignore_suppression);
  audio_engine->get_family(DRAudio::Family::FVideo)->set_volume(video_volume);
  audio_engine->get_family(DRAudio::Family::FVideo)->set_ignore_suppression(effect_ignore_suppression);
  audio_engine->get_family(DRAudio::Family::FBlip)->set_volume(blip_volume);
  audio_engine->get_family(DRAudio::Family::FBlip)->set_ignore_suppression(effect_ignore_suppression);

  { // ini swap
    cfg.beginGroup("character_ini");

    ini_map.clear();
    for (const QString &i_key : cfg.childKeys())
    {
      const QString i_value = cfg.value(i_key).toString();
      if (i_key == i_value || i_value.trimmed().isEmpty())
        continue;
      ini_map.insert(i_key, i_value);
    }

    cfg.endGroup();
  }

  // audio device
  update_favorite_device();

  // shortcuts
  screenshot_shortcut = QKeySequence(cfg.value("screenshot_shortcut", "Ctrl+S").toString());
  look_shortcut = QKeySequence(cfg.value("look_shortcut", "Ctrl+L").toString());
}

void AOConfigPrivate::save_file()
{
  cfg.setValue("autosave", autosave);

  { // notifications
    cfg.remove("notifications");
    cfg.beginGroup("notifications");
    for (int i = 0; i < notification_filter.length(); ++i)
      cfg.setValue(QString::number(i), notification_filter[i]);
    cfg.endGroup();
  }

  cfg.setValue("username", username);
  cfg.setValue("showname", showname);
  cfg.setValue("callwords", callwords);
  cfg.setValue("server_advertiser", server_advertiser);
  cfg.setValue("server_alerts", server_alerts);

  cfg.setValue("discord_presence", discord_presence);
  cfg.setValue("discord_hide_server", discord_hide_server);
  cfg.setValue("discord_hide_character", discord_hide_character);

  cfg.setValue("theme", theme);
  cfg.setValue("language", language);
  cfg.setValue("gamemode", manual_gamemode);
  cfg.setValue("manual_gamemode", manual_gamemode_selection);
  cfg.setValue("timeofday", manual_timeofday);
  cfg.setValue("manual_timeofday", manual_timeofday_selection);
  cfg.setValue("searchable_iniswap", searchable_iniswap);
  cfg.setValue("always_pre", always_pre);
  cfg.setValue("chat_tick_interval", chat_tick_interval);
  cfg.setValue("emote_preview", emote_preview);
  cfg.setValue("sticky_sfx", sticky_sfx);
  cfg.setValue("message_length_threshold", message_length_threshold);
  cfg.setValue("chatlog_limit", log_max_lines);
  cfg.setValue("chatlog_display_timestamp", log_display_timestamp);
  cfg.setValue("chatlog_display_client_id", log_display_client_id);
  cfg.setValue("chatlog_display_self_highlight", log_display_self_highlight);
  cfg.setValue("chatlog_newline", log_format_use_newline);
  cfg.setValue("chatlog_display_empty_messages", log_display_empty_messages);
  cfg.setValue("music_change_log", log_display_music_switch);
  cfg.setValue("chatlog_scrolldown", log_is_topdown);
  cfg.setValue("enable_logging", log_is_recording);

  // performance
  {
    cfg.remove("sprite_caching");
    cfg.beginGroup("sprite_caching");
    for (auto it = sprite_caching.cbegin(); it != sprite_caching.cend(); ++it)
    {
      const QString l_category_str = sprite_category_to_string(SpriteCategory(it.key()));
      cfg.setValue(l_category_str, it.value());
    }
    cfg.endGroup();
  }
  cfg.setValue("system_memory_threshold", system_memory_threshold);
  cfg.setValue("loading_bar_delay", loading_bar_delay);
  cfg.setValue("caching_threshold", caching_threshold);

  // audio
  if (favorite_device_driver.has_value())
    cfg.setValue("favorite_device_driver", favorite_device_driver.value());

  cfg.setValue("suppress_background_audio", suppress_background_audio);
  cfg.setValue("default_master", master_volume);
  cfg.setValue("default_system", system_volume);
  cfg.setValue("default_sfx", effect_volume);
  cfg.setValue("effect_ignore_suppression", effect_ignore_suppression);
  cfg.setValue("default_music", music_volume);
  cfg.setValue("music_ignore_suppression", music_ignore_suppression);
  cfg.setValue("default_video", video_volume);
  cfg.setValue("video_ignore_suppression", video_ignore_suppression);
  cfg.setValue("default_blip", blip_volume);
  cfg.setValue("blip_ignore_suppression", blip_ignore_suppression);
  cfg.setValue("blip_rate", blip_rate);
  cfg.setValue("punctuation_delay", punctuation_delay);
  cfg.setValue("theme_resize", theme_resize);
  cfg.setValue("fade_duration", fade_duration);
  cfg.setValue("blank_blips", blank_blips);

  cfg.remove("character_ini");
  { // ini swap
    cfg.beginGroup("character_ini");

    for (auto it = ini_map.cbegin(); it != ini_map.cend(); ++it)
      cfg.setValue(it.key(), it.value());

    cfg.endGroup();
  }

  // shortcuts
  cfg.setValue("screenshot_shortcut", screenshot_shortcut.toString());
  cfg.setValue("look_shortcut", look_shortcut.toString());

  cfg.sync();
}

void AOConfigPrivate::invoke_signal(QString p_method_name, QGenericArgument p_arg1, QGenericArgument p_arg2)
{
  for (QObject *i_child : qAsConst(children))
  {
    QMetaObject::invokeMethod(i_child, p_method_name.toStdString().c_str(), p_arg1, p_arg2);
  }
}

void AOConfigPrivate::update_favorite_device()
{
  if (!favorite_device_driver.has_value())
    return;
  audio_engine->set_favorite_device_driver(favorite_device_driver.value());
}

void AOConfigPrivate::on_application_state_changed(Qt::ApplicationState p_state)
{
  audio_engine->set_suppressed(suppress_background_audio && p_state != Qt::ApplicationActive);
}

// AOConfig ////////////////////////////////////////////////////////////////////

/*!
 * private classes are cool
 */
namespace
{
static QSharedPointer<AOConfigPrivate> d;
}

AOConfig::AOConfig(QObject *p_parent)
    : QObject(p_parent)
{
  // init if not created yet
  if (d == nullptr)
  {
    Q_ASSERT_X(qApp, "initialization", "QGuiApplication is required");
    d = QSharedPointer<AOConfigPrivate>(new AOConfigPrivate);
  }

  // ao2 is the pinnacle of thread security
  d->children.append(this);
}

AOConfig::~AOConfig()
{
  // totally safe!
  d->children.removeAll(this);
}

QString AOConfig::get_string(QString p_name, QString p_default) const
{
  return d->cfg.value(p_name, p_default).toString();
}

bool AOConfig::get_bool(QString p_name, bool p_default) const
{
  return d->cfg.value(p_name, p_default).toBool();
}

int AOConfig::get_number(QString p_name, int p_default) const
{
  return d->cfg.value(p_name, p_default).toInt();
}

bool AOConfig::first_launch() const
{
  return d->first_launch;
}

bool AOConfig::autosave() const
{
  return d->autosave;
}

bool AOConfig::display_notification(QString p_message) const
{
  return !d->notification_filter.contains(p_message, Qt::CaseInsensitive);
}

QString AOConfig::username() const
{
  return d->username;
}

QString AOConfig::showname() const
{
  return d->showname;
}

QString AOConfig::showname_placeholder() const
{
  return d->showname_placeholder;
}

QString AOConfig::character_ini(QString p_base_chr) const
{
  if (d->ini_map.contains(p_base_chr))
    return d->ini_map[p_base_chr];
  return p_base_chr;
}

QString AOConfig::callwords() const
{
  return d->callwords;
}

QString AOConfig::server_advertiser() const
{
  return d->server_advertiser;
}

bool AOConfig::server_alerts_enabled() const
{
  return d->server_alerts;
}

bool AOConfig::discord_presence() const
{
  return d->discord_presence;
}

bool AOConfig::discord_hide_server() const
{
  return d->discord_hide_server;
}

bool AOConfig::discord_hide_character() const
{
  return d->discord_hide_character;
}

QString AOConfig::language() const
{
  return d->language;
}

/**
 * @brief Return the current theme name.
 * @return Name of current theme.
 */
QString AOConfig::theme() const
{
  return d->theme;
}

QString AOConfig::gamemode() const
{
  return d->gamemode;
}

/**
 * @brief Return the current gamemode. If no gamemode is set, return the
 * empty string.
 *
 * @return Current gamemode, or empty string if not set.
 */
QString AOConfig::manual_gamemode() const
{
  return d->manual_gamemode;
}

/**
 * @brief Returns the current manual gamemode status.
 *
 * @details If true, a player can change gamemodes manually and their client
 * will ignore orders to change gamemode from the server. If false, neither is
 * possible and the client will follow orders from the server to change
 * gamemode.
 *
 * @return Current manual gamemode status.
 */
bool AOConfig::is_manual_gamemode_selection_enabled() const
{
  return d->manual_gamemode_selection;
}

QString AOConfig::timeofday() const
{
  return d->timeofday;
}

/**
 * @brief Returns the current manual time of day. If no time of day is set, return
 * the empty string.
 *
 * @return Current manual time of day, or empty string if not set.
 */
QString AOConfig::manual_timeofday() const
{
  return d->manual_timeofday;
}

/**
 * @brief Returns the current manual time of day status.
 *
 * @details If true, a player can change time of day manually and their client
 * will ignore orders to change time of day from the server. If false, neither
 * is possible and the client will follow orders from the server to change
 * time of day.
 *
 * @return Current manual time of day status.
 */
bool AOConfig::is_manual_timeofday_selection_enabled() const
{
  return d->manual_timeofday_selection;
}

bool AOConfig::searchable_iniswap_enabled() const
{
  return d->searchable_iniswap;
}

bool AOConfig::always_pre_enabled() const
{
  return d->always_pre;
}

int AOConfig::chat_tick_interval() const
{
  return d->chat_tick_interval;
}

bool AOConfig::emote_preview_enabled() const
{
  return d->emote_preview;
}

bool AOConfig::sticky_sfx_enabled() const
{
  return d->sticky_sfx;
}

int AOConfig::message_length_threshold() const
{
  return d->message_length_threshold;
}

int AOConfig::log_max_lines() const
{
  return d->log_max_lines;
}

bool AOConfig::log_display_timestamp_enabled() const
{
  return d->log_display_timestamp;
}

bool AOConfig::log_display_client_id_enabled() const
{
  return d->log_display_client_id;
}

bool AOConfig::log_display_self_highlight_enabled() const
{
  return d->log_display_self_highlight;
}

bool AOConfig::log_display_empty_messages_enabled() const
{
  return d->log_display_empty_messages;
}

bool AOConfig::log_is_topdown_enabled() const
{
  return d->log_is_topdown;
}

bool AOConfig::log_format_use_newline_enabled() const
{
  return d->log_format_use_newline;
}

bool AOConfig::log_display_music_switch_enabled() const
{
  return d->log_display_music_switch;
}

bool AOConfig::log_is_recording_enabled() const
{
  return d->log_is_recording;
}

int AOConfig::system_memory_threshold() const
{
  return d->system_memory_threshold;
}

bool AOConfig::sprite_caching_enabled(int type) const
{
  return d->sprite_caching[type];
}

int AOConfig::loading_bar_delay() const
{
  return d->loading_bar_delay;
}

int AOConfig::caching_threshold() const
{
  return d->caching_threshold;
}

std::optional<QString> AOConfig::favorite_device_driver() const
{
  return d->favorite_device_driver;
}

int AOConfig::master_volume() const
{
  return d->master_volume;
}

bool AOConfig::suppress_background_audio() const
{
  return d->suppress_background_audio;
}

int AOConfig::system_volume() const
{
  return d->system_volume;
}
int AOConfig::effect_volume() const
{
  return d->effect_volume;
}

bool AOConfig::effect_ignore_suppression() const
{
  return d->effect_ignore_suppression;
}

int AOConfig::music_volume() const
{
  return d->music_volume;
}

bool AOConfig::music_ignore_suppression() const
{
  return d->music_ignore_suppression;
}

int AOConfig::video_volume() const
{
  return d->video_volume;
}

bool AOConfig::video_ignore_suppression() const
{
  return d->video_ignore_suppression;
}

int AOConfig::blip_volume() const
{
  return d->blip_volume;
}

bool AOConfig::blip_ignore_suppression() const
{
  return d->blip_ignore_suppression;
}

int AOConfig::blip_rate() const
{
  return d->blip_rate;
}

int AOConfig::punctuation_delay() const
{
  return d->punctuation_delay;
}

bool AOConfig::blank_blips_enabled() const
{
  return d->blank_blips;
}

double AOConfig::theme_resize() const
{
  return d->theme_resize;
}

int AOConfig::fade_duration() const
{
  return d->fade_duration;
}

QKeySequence AOConfig::screenshot_shortcut() const
{
  return d->screenshot_shortcut;
}

QKeySequence AOConfig::look_shortcut() const
{
  return d->look_shortcut;
}

void AOConfig::load_file()
{
  d->load_file();
}

void AOConfig::save_file()
{
  d->save_file();
}

void AOConfig::set_autosave(bool p_enabled)
{
  if (d->autosave == p_enabled)
    return;
  d->autosave = p_enabled;
  d->invoke_signal("autosave_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::clear_notification_filter()
{
  d->notification_filter.clear();
}

void AOConfig::filter_notification(QString p_message)
{
  d->notification_filter.append(p_message);
}

void AOConfig::set_username(QString p_value)
{
  const QString l_simplified_value = p_value.simplified();
  if (d->username == l_simplified_value)
    return;
  d->username = l_simplified_value;
  d->invoke_signal("username_changed", Q_ARG(QString, d->username));
}

void AOConfig::set_showname(QString p_value)
{
  const QString l_simplified_value = p_value.simplified();
  if (d->showname == l_simplified_value && !l_simplified_value.isEmpty())
    return;
  d->showname = l_simplified_value;
  d->invoke_signal("showname_changed", Q_ARG(QString, d->showname));
}

void AOConfig::set_showname_placeholder(QString p_string)
{
  if (d->showname_placeholder == p_string)
    return;
  d->showname_placeholder = p_string;
  d->invoke_signal("showname_placeholder_changed", Q_ARG(QString, p_string));
}

void AOConfig::clear_showname_placeholder()
{
  set_showname_placeholder(nullptr);
}

void AOConfig::set_character_ini(QString p_base_chr, QString p_target_chr)
{
  if (d->ini_map.contains(p_base_chr))
  {
    if (d->ini_map[p_base_chr] == p_target_chr)
      return;
  }
  else if (p_base_chr == p_target_chr)
    return;
  if (p_base_chr == p_target_chr)
    d->ini_map.remove(p_base_chr);
  else
    d->ini_map.insert(p_base_chr, p_target_chr);
  d->invoke_signal("character_ini_changed", Q_ARG(QString, p_base_chr));
}

void AOConfig::set_callwords(QString p_string)
{
  if (d->callwords == p_string)
    return;
  d->callwords = p_string;
  d->invoke_signal("callwords_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_server_advertiser(QString p_address)
{
  if (d->server_advertiser == p_address)
    return;
  d->server_advertiser = p_address;
  d->invoke_signal("server_advertiser_changed", Q_ARG(QString, p_address));
}

void AOConfig::set_server_alerts(bool p_enabled)
{
  if (d->server_alerts == p_enabled)
    return;
  d->server_alerts = p_enabled;
  d->invoke_signal("server_alerts_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_discord_presence(const bool p_enabled)
{
  if (d->discord_presence == p_enabled)
    return;
  d->discord_presence = p_enabled;
  d->invoke_signal("discord_presence_changed", Q_ARG(bool, d->discord_presence));
}

void AOConfig::set_discord_hide_server(const bool p_enabled)
{
  if (d->discord_hide_server == p_enabled)
    return;
  d->discord_hide_server = p_enabled;
  d->invoke_signal("discord_hide_server_changed", Q_ARG(bool, d->discord_hide_server));
}

void AOConfig::set_discord_hide_character(const bool p_enabled)
{
  if (d->discord_hide_character == p_enabled)
    return;
  d->discord_hide_character = p_enabled;
  d->invoke_signal("discord_hide_character_changed", Q_ARG(bool, d->discord_hide_character));
}

void AOConfig::setLanguage(QString t_language)
{
  if(d->language == t_language) return;
  d->language = t_language;
  d->invoke_signal("language_changed", Q_ARG(QString, t_language));
}

void AOConfig::set_theme(QString p_string)
{
  if (d->theme == p_string)
    return;
  d->theme = p_string;
  d->manual_gamemode.clear();
  d->manual_timeofday.clear();
  d->invoke_signal("theme_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_gamemode(QString p_string)
{
  if (d->gamemode == p_string)
    return;
  d->gamemode = p_string;
  ThemeManager::get().LoadGamemode(p_string);
  d->invoke_signal("gamemode_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_manual_gamemode(QString p_string)
{
  if (d->manual_gamemode == p_string)
    return;
  d->manual_gamemode = p_string;
  ThemeManager::get().LoadGamemode(p_string);
  d->manual_timeofday.clear();
  d->invoke_signal("manual_gamemode_changed", Q_ARG(QString, p_string));
}

void AOConfig::set_manual_gamemode_selection_enabled(bool p_enabled)
{
  if (d->manual_gamemode_selection == p_enabled)
    return;
  d->manual_gamemode_selection = p_enabled;
  d->invoke_signal("manual_gamemode_selection_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_timeofday(QString p_string)
{
  if (d->timeofday == p_string)
    return;
  d->timeofday = p_string;
  d->invoke_signal("timeofday_changed", Q_ARG(QString, p_string));
  VariableManager::get().setVariable("time_period", p_string);
  ReplayManager::get().RecordChangeTOD(p_string);
}

void AOConfig::set_manual_timeofday(QString p_string)
{
  if (d->manual_timeofday == p_string)
    return;
  d->manual_timeofday = p_string;
  d->invoke_signal("manual_timeofday_changed", Q_ARG(QString, p_string));
  VariableManager::get().setVariable("time_period", p_string);
  ReplayManager::get().RecordChangeTOD(p_string);
}

void AOConfig::set_manual_timeofday_selection_enabled(bool p_enabled)
{
  if (d->manual_timeofday_selection == p_enabled)
    return;
  d->manual_timeofday_selection = p_enabled;
  d->invoke_signal("manual_timeofday_selection_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_searchable_iniswap(bool p_enabled)
{
  if (d->searchable_iniswap == p_enabled)
    return;
  d->searchable_iniswap = p_enabled;
  d->invoke_signal("searchable_iniswap_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_always_pre(bool p_enabled)
{
  if (d->always_pre == p_enabled)
    return;
  d->always_pre = p_enabled;
  d->invoke_signal("always_pre_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_chat_tick_interval(int p_number)
{
  if (d->chat_tick_interval == p_number)
    return;
  d->chat_tick_interval = p_number;
  d->invoke_signal("chat_tick_interval_changed", Q_ARG(int, p_number));
}

void AOConfig::set_emote_preview(bool p_enabled)
{
  if (d->emote_preview == p_enabled)
    return;
  d->emote_preview = p_enabled;
  d->invoke_signal("emote_preview_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_sticky_sfx(bool p_enabled)
{
  if (d->sticky_sfx == p_enabled)
    return;
  d->sticky_sfx = p_enabled;
  d->invoke_signal("sticky_sfx_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_message_length_threshold(int p_number)
{
  if (d->message_length_threshold == p_number)
    return;
  d->message_length_threshold = p_number;
  d->invoke_signal("message_length_threshold_changed", Q_ARG(int, p_number));
}

void AOConfig::set_log_max_lines(int p_number)
{
  if (d->log_max_lines == p_number)
    return;
  d->log_max_lines = p_number;
  d->invoke_signal("log_max_lines_changed", Q_ARG(int, p_number));
}

void AOConfig::set_log_display_timestamp(bool p_enabled)
{
  if (d->log_display_timestamp == p_enabled)
    return;
  d->log_display_timestamp = p_enabled;
  d->invoke_signal("log_display_timestamp_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_display_client_id(bool p_enabled)
{
  if (d->log_display_client_id == p_enabled)
    return;
  d->log_display_client_id = p_enabled;
  d->invoke_signal("log_display_client_id_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_display_self_highlight(bool p_enabled)
{
  if (d->log_display_self_highlight == p_enabled)
    return;
  d->log_display_self_highlight = p_enabled;
  d->invoke_signal("log_display_self_highlight_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_display_empty_messages(bool p_enabled)
{
  if (d->log_display_empty_messages == p_enabled)
    return;
  d->log_display_empty_messages = p_enabled;
  d->invoke_signal("log_display_empty_messages_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_format_use_newline(bool p_enabled)
{
  if (d->log_format_use_newline == p_enabled)
    return;
  d->log_format_use_newline = p_enabled;
  d->invoke_signal("log_format_use_newline_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_is_topdown(bool p_enabled)
{
  if (d->log_is_topdown == p_enabled)
    return;
  d->log_is_topdown = p_enabled;
  d->invoke_signal("log_is_topdown_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_display_music_switch(bool p_enabled)
{
  if (d->log_display_music_switch == p_enabled)
    return;
  d->log_display_music_switch = p_enabled;
  d->invoke_signal("log_display_music_switch_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_log_is_recording(bool p_enabled)
{
  if (d->log_is_recording == p_enabled)
    return;
  d->log_is_recording = p_enabled;
  d->invoke_signal("log_is_recording_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_master_volume(int p_number)
{
  if (d->master_volume == p_number)
    return;
  d->master_volume = p_number;
  d->audio_engine->set_volume(p_number);
  d->invoke_signal("master_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_suppress_background_audio(bool p_enabled)
{
  if (d->suppress_background_audio == p_enabled)
    return;
  d->suppress_background_audio = p_enabled;
  d->invoke_signal("suppress_background_audio_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_system_memory_threshold(int p_percent)
{
  p_percent = qBound(10, p_percent, 80);
  if (d->system_memory_threshold == p_percent)
    return;
  d->system_memory_threshold = p_percent;
  mk2::SpriteDynamicReader::set_system_memory_threshold(p_percent);
  d->invoke_signal("system_memory_threshold_changed", Q_ARG(int, p_percent));
}

void AOConfig::set_sprite_caching(int p_type, bool p_enabled)
{
  if (d->sprite_caching[p_type] == p_enabled)
    return;
  d->sprite_caching[p_type] = p_enabled;
  d->invoke_signal("sprite_caching_toggled", Q_ARG(int, p_type), Q_ARG(bool, p_enabled));
}

void AOConfig::set_loading_bar_delay(int p_delay)
{
  p_delay = qBound(0, p_delay, 2000);
  if (d->loading_bar_delay == p_delay)
    return;
  d->loading_bar_delay = p_delay;
  d->invoke_signal("loading_bar_delay_changed", Q_ARG(int, p_delay));
}

void AOConfig::set_caching_threshold(int p_percent)
{
  p_percent = qBound(0, p_percent, 100);
  if (d->caching_threshold == p_percent)
    return;
  d->caching_threshold = p_percent;
  d->invoke_signal("caching_threshold_changed", Q_ARG(int, p_percent));
}

void AOConfig::set_favorite_device_driver(QString p_device_driver)
{
  if (d->favorite_device_driver.has_value() && d->favorite_device_driver.value() == p_device_driver)
    return;
  d->favorite_device_driver = p_device_driver;
  d->update_favorite_device();
  d->invoke_signal("favorite_device_changed", Q_ARG(QString, p_device_driver));
}

void AOConfig::set_system_volume(int p_number)
{
  if (d->system_volume == p_number)
    return;
  d->system_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FSystem)->set_volume(p_number);
  d->invoke_signal("system_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_effect_volume(int p_number)
{
  if (d->effect_volume == p_number)
    return;
  d->effect_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FEffect)->set_volume(p_number);
  d->invoke_signal("effect_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_effect_ignore_suppression(bool p_enabled)
{
  if (d->effect_ignore_suppression == p_enabled)
    return;
  d->effect_ignore_suppression = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FEffect)->set_ignore_suppression(p_enabled);
  d->invoke_signal("effect_ignore_suppression_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_music_volume(int p_number)
{
  if (d->music_volume == p_number)
    return;
  d->music_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FMusic)->set_volume(p_number);
  d->invoke_signal("music_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_music_ignore_suppression(bool p_enabled)
{
  if (d->music_ignore_suppression == p_enabled)
    return;
  d->music_ignore_suppression = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FMusic)->set_ignore_suppression(p_enabled);
  d->invoke_signal("music_ignore_suppression_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_video_volume(int p_number)
{
  if (d->video_volume == p_number)
    return;
  d->video_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FVideo)->set_volume(p_number);
  d->invoke_signal("video_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_video_ignore_suppression(bool p_enabled)
{
  if (d->video_ignore_suppression == p_enabled)
    return;
  d->video_ignore_suppression = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FVideo)->set_ignore_suppression(p_enabled);
  d->invoke_signal("video_ignore_suppression_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_blip_volume(int p_number)
{
  if (d->blip_volume == p_number)
    return;
  d->blip_volume = p_number;
  d->audio_engine->get_family(DRAudio::Family::FBlip)->set_volume(p_number);
  d->invoke_signal("blip_volume_changed", Q_ARG(int, p_number));
}

void AOConfig::set_blip_ignore_suppression(bool p_enabled)
{
  if (d->blip_ignore_suppression == p_enabled)
    return;
  d->blip_ignore_suppression = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FBlip)->set_ignore_suppression(p_enabled);
  d->invoke_signal("blip_ignore_suppression_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::set_blip_rate(int p_number)
{
  if (d->blip_rate == p_number)
    return;
  d->blip_rate = p_number;
  d->invoke_signal("blip_rate_changed", Q_ARG(int, p_number));
}

void AOConfig::set_punctuation_delay(int p_number)
{
  if (d->punctuation_delay == p_number)
    return;
  d->punctuation_delay = p_number;
  d->invoke_signal("punctuation_delay_changed", Q_ARG(int, p_number));
}

void AOConfig::set_blank_blips(bool p_enabled)
{
  if (d->blank_blips == p_enabled)
    return;
  d->blank_blips = p_enabled;
  d->audio_engine->get_family(DRAudio::Family::FBlip)->set_ignore_suppression(p_enabled);
  d->invoke_signal("blank_blips_changed", Q_ARG(bool, p_enabled));
}

void AOConfig::setThemeResize(double resize)
{
  if (d->theme_resize == resize)
    return;
  d->theme_resize = resize;
  ThemeManager::get().SetResizeClient(resize);
  d->invoke_signal("theme_resize_changed", Q_ARG(double, resize));
}

void AOConfig::setFadeDuration(int duration)
{
  if (d->fade_duration == duration)
    return;
  d->fade_duration = duration;
  SceneManager::get().setFadeDuration(duration);
  d->invoke_signal("fade_duration_changed", Q_ARG(int, duration));
}

void AOConfig::set_screenshot_shortcut(const QKeySequence &p_shortcut)
{
  if (d->screenshot_shortcut == p_shortcut)
    return;
  d->screenshot_shortcut = p_shortcut;
  d->invoke_signal("shortcuts_changed");
}

void AOConfig::set_look_shortcut(const QKeySequence &p_shortcut)
{
  if (d->look_shortcut == p_shortcut)
    return;
  d->look_shortcut = p_shortcut;
  d->invoke_signal("shortcuts_changed");
}

// moc
#include "aoconfig.moc"
