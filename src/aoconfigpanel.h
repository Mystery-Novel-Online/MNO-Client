#ifndef AOCONFIGPANEL_H
#define AOCONFIGPANEL_H

#include "draudioengine.h"
#include "qlistwidget.h"

class AOApplication;
class AOConfig;

#include <QDoubleSpinBox>
#include <QMap>
#include <QWidget>
#include <qtableview.h>
#include <qtreeview.h>

class QCheckBox;
class QComboBox;
class QGroupBox;
class QKeySequenceEdit;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QSlider;
class QSpinBox;

class AOConfigPanel : public QWidget
{
  Q_OBJECT

public:
  AOConfigPanel(AOApplication *p_ao_app, QWidget *p_parent = nullptr);

public slots:
  void on_config_reload_theme_requested();

signals:
  void reload_theme();
  void reload_character();
  void reload_audiotracks();

  void emit_sprite_caching_toggled(int type, bool enabled);

protected:
  void showEvent(QShowEvent *event) override;

private:
  void refreshLanguageList();
  void refresh_theme_list();
  void refresh_packages_list();
  void refresh_gamemode_list();
  void refresh_timeofday_list();
  void update_audio_device_list();

private slots:
  void updateLanguage(QString t_data);
  void update_theme_controls();
  void on_switch_theme_clicked();
  void on_reload_theme_clicked();
  void on_reload_character_clicked();
  void on_reload_audiotracks_clicked();
  void on_load_packages_clicked();
  void on_check_for_updates_clicked();
  void on_theme_changed(QString);
  void on_gamemode_changed(QString);
  void on_manual_gamemode_selection_changed(bool);
  void on_manual_gamemode_changed(QString);
  void on_manual_gamemode_index_changed(QString p_text);
  void on_timeofday_changed(QString);
  void on_manual_timeofday_selection_changed(bool);
  void on_manual_timeofday_changed(QString);
  void on_manual_timeofday_index_changed(QString p_text);
  void on_showname_placeholder_changed(QString p_text);
  void on_log_is_topdown_changed(bool p_enabled);
  void on_device_current_index_changed(int p_index);
  void on_audio_device_changed(DRAudioDevice p_device);
  void on_favorite_audio_device_changed(DRAudioDevice p_device);
  void on_audio_device_list_changed(QVector<DRAudioDevice> p_device_list);

  void on_volume_value_changed(int p_num);

  void on_length_threshold_value_changed(int p_number);

  // performance
  void set_sprite_caching_toggled(int type, bool enabled);
  void handle_sprite_caching_toggled(bool enabled);
  void set_system_memory_threshold(int megabytes);
  void set_loading_bar_delay(int percent);
  void set_caching_threshold(int msecs);

private:
  // FIXME This dependency shouldn't have come to exist.
  AOApplication *ao_app = nullptr;
  // driver
  AOConfig *m_config = nullptr;
  DRAudioEngine *m_engine = nullptr;

  //tabs
  QListView *pTabFilters = nullptr;
  QTabWidget *tab_widget;

  // behaviour
  QPushButton *ui_save = nullptr;
  QPushButton *ui_close = nullptr;
  QCheckBox *ui_autosave = nullptr;

  // notifications
  QPushButton *ui_clear_notifications = nullptr;

  // general
  QLineEdit *ui_username = nullptr;
  QLineEdit *ui_callwords = nullptr;
  QLineEdit *ui_advertiser = nullptr;
  QCheckBox *ui_server_alerts = nullptr;
  QGroupBox *ui_discord_presence = nullptr;
  QCheckBox *ui_discord_hide_server = nullptr;
  QCheckBox *ui_discord_hide_character = nullptr;

  // character
  QLineEdit *ui_showname = nullptr;
  QPushButton *ui_reload_character = nullptr;
  QCheckBox *ui_searchable_iniswap = nullptr;

  // emotes
  QCheckBox *ui_emote_preview = nullptr;
  QCheckBox *ui_always_pre = nullptr;
  QCheckBox *ui_sticky_sfx = nullptr;

  // dialog
  QSpinBox *ui_chat_tick_interval = nullptr;
  QSpinBox *ui_blip_rate = nullptr;
  QCheckBox *ui_blank_blips = nullptr;
  QSpinBox *ui_punctuation_delay = nullptr;

  // game
  QComboBox *ui_theme = nullptr;
  QLabel *wLanguageCredits = nullptr;
  QComboBox *wSettingsLanguage = nullptr;

  QPushButton *ui_switch_theme = nullptr;
  QPushButton *ui_reload_theme = nullptr;
  QTreeView *ui_themeModules = nullptr;
  QLineEdit *ui_gamemode = nullptr;
  QComboBox *ui_manual_gamemode = nullptr;
  QCheckBox *ui_manual_gamemode_selection = nullptr;
  QLineEdit *ui_timeofday = nullptr;
  QComboBox *ui_manual_timeofday = nullptr;
  QCheckBox *ui_manual_timeofday_selection = nullptr;

  QKeySequenceEdit *ui_screenshot_shortcut = nullptr;
  QKeySequenceEdit *ui_look_shortcut = nullptr;

  // messages
  QSlider *ui_length_threshold = nullptr;
  QLabel *ui_length_threshold_label = nullptr;

  // chatlog
  QSpinBox *ui_log_max_lines = nullptr;
  QCheckBox *ui_log_display_timestamp = nullptr;
  QCheckBox *ui_log_display_client_id = nullptr;
  QCheckBox *ui_log_display_self_highlight = nullptr;
  QCheckBox *ui_log_format_use_newline = nullptr;
  QCheckBox *ui_log_display_empty_messages = nullptr;
  QCheckBox *ui_log_display_music_switch = nullptr;
  QRadioButton *ui_log_orientation_top_down = nullptr;
  QRadioButton *ui_log_orientation_bottom_up = nullptr;
  QCheckBox *ui_log_is_recording = nullptr;

  // performance
  QCheckBox *ui_cache_backgrounds;
  QCheckBox *ui_cache_characters;
  QCheckBox *ui_cache_effects;
  QCheckBox *ui_cache_shouts;
  QCheckBox *ui_cache_gui;
  QCheckBox *ui_cache_stickers;
  QMap<int, QCheckBox *> m_cache_checkbox_map;

  QSlider *ui_system_memory_threshold;
  QLabel *ui_system_memory_threshold_label;
  QSlider *ui_loading_bar_delay;
  QLabel *ui_loading_bar_delay_label;
  QSlider *ui_caching_threshold;
  QLabel *ui_caching_threshold_label;

  // audio
  QComboBox *ui_device = nullptr;
  QCheckBox *ui_favorite_device = nullptr;

  //Audio Sliders
  QMap<QSlider*, QLabel*> volumeSliderMap = {};
  QSlider *ui_master = nullptr;
  QLabel *ui_master_value = nullptr;
  QGroupBox *ui_suppress_background_audio = nullptr;
  QSlider *ui_system = nullptr;
  QLabel *ui_system_value = nullptr;
  QSlider *ui_effect = nullptr;
  QCheckBox *ui_effect_ignore_suppression = nullptr;
  QLabel *ui_effect_value = nullptr;
  QSlider *ui_music = nullptr;
  QCheckBox *ui_music_ignore_suppression = nullptr;
  QLabel *ui_music_value = nullptr;
  QSlider *ui_video = nullptr;
  QCheckBox *ui_video_ignore_suppression = nullptr;
  QLabel *ui_video_value = nullptr;
  QSlider *ui_blip = nullptr;
  QCheckBox *ui_blip_ignore_suppression = nullptr;
  QLabel *ui_blip_value = nullptr;
  QPushButton *ui_reload_audiotracks = nullptr;

  // about
  QLabel *ui_about = nullptr;

  // packages
  QListWidget *ui_packages_list = nullptr;
  QPushButton *ui_load_new_packages = nullptr;

  //Updates
  QCheckBox *ui_beta_updates = nullptr;
  QPushButton *ui_check_updates = nullptr;

  //Theme
  QDoubleSpinBox *ui_theme_resize = nullptr;
  QSpinBox *ui_fade_duration = nullptr;


private slots:
  void updateTabsVisibility(const QModelIndex &current, const QModelIndex &previous);
  void username_editing_finished();
  void showname_editing_finished();
  void advertiser_editing_finished();
  void callwords_editing_finished();
  void shortcuts_editing_finished();
};

#endif // AOCONFIGPANEL_H
