#ifndef AOCONFIG_H
#define AOCONFIG_H

#include "datatypes.h"

#include <QObject>

#include <optional>

class AOConfig : public QObject
{
  Q_OBJECT

public:
  AOConfig(QObject *p_parent = nullptr);
  ~AOConfig();

  // generic getters
  QString get_string(QString p_name, QString p_default = nullptr) const;
  bool get_bool(QString p_name, bool p_default = false) const;
  int get_number(QString p_name, int p_default = 0) const;

  bool first_launch() const;

  // getters
  bool autosave() const;
  bool display_notification(QString message) const;
  QString username() const;
  QString showname() const;
  QString showname_placeholder() const;
  QString character_ini(QString base_character) const;
  QString callwords() const;
  QString server_advertiser() const;
  bool server_alerts_enabled() const;
  bool discord_presence() const;
  bool discord_hide_server() const;
  bool discord_hide_character() const;
  QString language() const;
  QString theme() const;
  QString gamemode() const;
  QString manual_gamemode() const;
  bool is_manual_gamemode_selection_enabled() const;
  QString timeofday() const;
  QString manual_timeofday() const;
  bool is_manual_timeofday_selection_enabled() const;
  bool searchable_iniswap_enabled() const;
  bool always_pre_enabled() const;
  int chat_tick_interval() const;
  bool emote_preview_enabled() const;
  bool sticky_sfx_enabled() const;
  int message_length_threshold() const;
  int log_max_lines() const;
  bool log_display_timestamp_enabled() const;
  bool log_display_client_id_enabled() const;
  bool log_display_self_highlight_enabled() const;
  bool log_display_empty_messages_enabled() const;
  bool log_is_topdown_enabled() const;
  bool log_format_use_newline_enabled() const;
  bool log_display_music_switch_enabled() const;
  bool log_is_recording_enabled() const;

  // performance
  bool sprite_caching_enabled(int type) const;
  int system_memory_threshold() const;
  int loading_bar_delay() const;
  int caching_threshold() const;

  // audio
  std::optional<QString> favorite_device_driver() const;
  int master_volume() const;
  bool suppress_background_audio() const;
  int system_volume() const;
  int effect_volume() const;
  bool effect_ignore_suppression() const;
  int music_volume() const;
  bool music_ignore_suppression() const;
  int video_volume() const;
  bool video_ignore_suppression() const;
  int blip_volume() const;
  bool blip_ignore_suppression() const;
  int blip_rate() const;
  int punctuation_delay() const;
  bool blank_blips_enabled() const;

  double theme_resize() const;
  int fade_duration() const;

  // shortcuts
  QKeySequence screenshot_shortcut() const;
  QKeySequence look_shortcut() const;

  // io
public slots:
  void load_file();
  void save_file();

  // setters
public slots:
  void set_autosave(bool p_enabled);
  void clear_notification_filter();
  void filter_notification(QString message);
  void set_username(QString p_string);
  void set_showname(QString p_string);
  void set_showname_placeholder(QString p_string);
  void clear_showname_placeholder();
  void set_character_ini(QString base_character, QString target_character);
  void set_callwords(QString p_string);
  void set_server_advertiser(QString address);
  void set_server_alerts(bool p_enabled);
  void set_discord_presence(const bool p_enabled);
  void set_discord_hide_server(const bool p_enabled);
  void set_discord_hide_character(const bool p_enabled);
  void setLanguage(QString t_language);
  void set_theme(QString p_string);
  void set_gamemode(QString p_string);
  void set_manual_gamemode(QString p_string);
  void set_manual_gamemode_selection_enabled(bool p_enabled);
  void set_timeofday(QString p_string);
  void set_manual_timeofday(QString p_string);
  void set_manual_timeofday_selection_enabled(bool p_enabled);
  void set_searchable_iniswap(bool);
  void set_always_pre(bool p_enabled);
  void set_chat_tick_interval(int p_number);
  void set_emote_preview(bool p_enabled);
  void set_sticky_sfx(bool p_enabled);
  void set_message_length_threshold(int percent);
  void set_log_max_lines(int p_number);
  void set_log_display_timestamp(bool p_enabled);
  void set_log_display_client_id(bool p_enabled);
  void set_log_display_self_highlight(bool p_enabled);
  void set_log_display_empty_messages(bool p_enabled);
  void set_log_is_topdown(bool p_enabled);
  void set_log_format_use_newline(bool p_enabled);
  void set_log_display_music_switch(bool p_enabled);
  void set_log_is_recording(bool p_enabled);
  void set_suppress_background_audio(bool p_enabled);

  // performance
  void set_sprite_caching(int type, bool enabled);
  void set_system_memory_threshold(int percent);
  void set_loading_bar_delay(int delay);
  void set_caching_threshold(int percent);

  // audio
  void set_favorite_device_driver(QString p_device_driver);
  void set_master_volume(int p_number);
  void set_system_volume(int p_number);
  void set_effect_volume(int p_number);
  void set_effect_ignore_suppression(bool p_enabled);
  void set_music_volume(int p_number);
  void set_music_ignore_suppression(bool p_enabled);
  void set_video_volume(int p_number);
  void set_video_ignore_suppression(bool p_enabled);
  void set_blip_volume(int p_number);
  void set_blip_ignore_suppression(bool p_enabled);
  void set_blip_rate(int p_number);
  void set_punctuation_delay(int p_number);
  void set_blank_blips(bool p_enabled);
  void setThemeResize(double resize);
  void setFadeDuration(int duration);

  // shortcuts
  void set_screenshot_shortcut(const QKeySequence &p_shortcut);
  void set_look_shortcut(const QKeySequence &p_shortcut);

  // signals
signals:
  // meta
  void autosave_changed(bool);

  // general
  void username_changed(QString);
  void callwords_changed(QString);
  void server_advertiser_changed(QString);
  void server_alerts_changed(bool);
  void discord_presence_changed(bool);
  void discord_hide_server_changed(bool);
  void discord_hide_character_changed(bool);

  // game
  void showname_changed(QString);
  void showname_placeholder_changed(QString);
  void character_ini_changed(QString base_character);
  void searchable_iniswap_changed(bool);
  void always_pre_changed(bool);
  void chat_tick_interval_changed(int);
  void emote_preview_changed(bool);
  void sticky_sfx_changed(bool);

  // theme
  void language_changed(QString);
  void theme_changed(QString);
  void gamemode_changed(QString);
  void manual_gamemode_changed(QString);
  void manual_gamemode_selection_changed(bool);
  void timeofday_changed(QString);
  void manual_timeofday_changed(QString);
  void manual_timeofday_selection_changed(bool);

  // ic
  void message_length_threshold_changed(int);

  // log
  void log_max_lines_changed(int);
  void log_display_timestamp_changed(bool);
  void log_display_client_id_changed(bool);
  void log_display_self_highlight_changed(bool);
  void log_display_empty_messages_changed(bool);
  void log_is_topdown_changed(bool);
  void log_format_use_newline_changed(bool);
  void log_display_music_switch_changed(bool);
  void log_is_recording_changed(bool);

  // performance
  void sprite_caching_toggled(int, bool);
  void system_memory_threshold_changed(int);
  void loading_bar_delay_changed(int);
  void caching_threshold_changed(int);

  // audio
  void favorite_device_changed(QString);
  void master_volume_changed(int);
  void suppress_background_audio_changed(bool);
  void system_volume_changed(int);
  void effect_volume_changed(int);
  void effect_ignore_suppression_changed(bool);
  void music_volume_changed(int);
  void music_ignore_suppression_changed(bool);
  void video_volume_changed(int);
  void video_ignore_suppression_changed(bool);
  void blip_volume_changed(int);
  void blip_ignore_suppression_changed(bool);
  void blip_rate_changed(int);
  void punctuation_delay_changed(int);
  void blank_blips_changed(bool);

  //Theme
  void theme_resize_changed(double);
  void fade_duration_changed(int);

  // shortcuts
  void shortcuts_changed();
};

#endif // AOCONFIG_H
