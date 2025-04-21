#ifndef AOAPPLICATION_H
#define AOAPPLICATION_H

#include "datatypes.h"
#include "drpacket.h"
#include "drserversocket.h"

class AOConfig;
class AOConfigPanel;
class Courtroom;
class DRDiscord;
class DRTheme;
class DRMasterClient;
class Lobby;

#include <QApplication>
#include <QVector>

#include <optional>

class AOApplication : public QApplication
{
  Q_OBJECT

public:
  enum ServerStatus
  {
    NotConnected,
    Connecting,
    Connected,
    Joined,
    TimedOut,
    Disconnected,
  };

  static AOApplication *m_Instance;
  static AOApplication *getInstance();

  AOApplication(int &argc, char **argv);
  ~AOApplication();

  int get_client_id() const;
  void set_client_id(int id);

  void leave_server();
  void connect_to_server(DRServerInfo server);
  void send_server_packet(DRPacket packet);
  ServerStatus last_server_status();
  bool joined_server();

  Lobby *get_lobby() const;
  void construct_lobby();
  void destruct_lobby();

  Courtroom *get_courtroom() const;
  void construct_courtroom();
  void destruct_courtroom();

  DRDiscord *get_discord() const;

  DRTheme *current_theme = nullptr;

  VersionNumber get_server_client_version() const;
  VersionStatus get_server_client_version_status() const;
  bool is_server_client_version_compatible() const;

  ///////////////////////////////////////////

  // Returns the character the player has currently selected
  QString get_current_char();

  // implementation in path_functions.cpp
  QVector<QString> package_names;
  QVector<QString> m_disabled_packages = {};
  void reload_packages();
  void save_disabled_packages_ini();
  void read_disabled_packages_ini();
  QString get_base_path();
  QString get_package_path(QString p_package);
  QString get_package_or_base_path(QString p_path);
  QString get_package_or_base_file(QString p_filepath);
  QVector<QString> get_all_package_and_base_paths(QString p_path);
  QString get_base_file_path(QString file);
  QString get_character_folder_path(QString character);
  QString get_character_path(QString p_character, QString p_file);
  // QString get_demothings_path();
  QString get_music_folder_path();
  QString get_music_path(QString p_song);

  QString get_background_path(QString p_background_name);
  QString get_background_dir_path(QString p_identifier);

  bool is_safe_path(QString p_file);

  QString find_asset_path(QStringList file_list, QStringList extension_list);
  QString find_asset_path(QStringList file_list);
  QString find_asset_path(QString p_file, QStringList p_extension_list);
  QString find_asset_path(QString p_file);
  QString find_theme_asset_path(QString file, QStringList extension_list);
  QString find_theme_asset_path(QString file);
  QString find_current_theme_path();

  QString getCurrentTheme();

  QString get_case_sensitive_path(QString p_file);

  ////// Functions for accessing the config panel //////
  void toggle_config_panel();

  ////// Functions for reading and writing files //////
  // Implementations file_functions.cpp

  // Returns text from note file
  QString read_note(QString filename);

  // returns a list of call words
  QStringList get_callwords();

  // returns whatever the client should simulate first person dialog
  bool get_first_person_enabled();

  // TODO document what this does
  QStringList get_sfx_list();

  // Writes to note file
  void write_note(QString p_text, QString filename);

  // appends to note file
  void append_note(QString p_line, QString filename);

  QString read_ini(QString p_identifier, QString p_path);

  QString read_theme_ini(QString p_identifier, QString p_file);

  bool read_theme_ini_bool(QString p_identifier, QString p_file);

  int read_theme_ini_int(QString p_identifier, QString p_file);

  // Returns the coordinates of widget with p_identifier from p_file
  QPoint get_button_spacing(QString p_identifier, QString p_file);

  // Returns the dimensions of widget with specified identifier from p_file
  pos_size_type get_element_dimensions(QString p_identifier, QString p_file);

  // Returns the value of font property p_identifier from p_file
  int get_font_property(QString p_identifier, QString p_file);

  // Returns the name of the font with p_identifier from p_file
  QString get_font_name(QString p_identifier, QString p_file);

  // Returns the color with p_identifier from p_file
  std::optional<QColor> maybe_color(QString p_identifier, QString p_file);

  // Returns the color with p_identifier from p_file
  QColor get_color(QString p_identifier, QString p_file);

  // Returns the sfx with p_identifier from sounds.ini in the current theme path
  QString get_sfx(QString p_identifier);

  // Returns the value of p_search_line within target_tag and terminator_tag

  QVariant read_char_ini(QString character, QString group, QString key, QVariant default_value);
  QVariant read_char_ini(QString character, QString group, QString key);

  // Returns the text between target_tag and terminator_tag in p_file
  QString get_stylesheet(QString target_tag, QString p_file);

  // Returns string list (color name to color HEX)
  QMap<DR::Color, DR::ColorInfo> get_chatmessage_colors();

  // Returns string list (highlight character to color name)
  QVector<QStringList> get_highlight_colors();

  // Returns special button on cc_config according to index
  QString get_spbutton(QString p_tag, int index);

  // Returns effect on cc_config according to index
  QStringList get_effect(int index);

  // Returns the showname from the ini of p_char
  QString get_showname(QString p_char);

  // Returns the value of chat from the specific p_char's ini file
  QString get_chat(QString p_char);

  // Returns the name of p_char
  QString get_char_name(QString p_char);

  QStringList get_char_include(QString character);

  QStringList get_char_include_tree(QString character);

  // Returns p_char's gender
  QString get_gender(QString p_char);

  QVector<DREmote> get_emote_list(QString p_chr);

  // Returns x,y offset for effect p_effect
  QStringList get_effect_offset(QString p_char, int p_effect);

  // Returns overlay at p_effect in char_path/overlay
  QStringList get_overlay(QString p_char, int p_effect);

  ////// Functions for fonts handling //////
  void load_fonts();
  Courtroom *m_courtroom = nullptr;

public slots:
  void loading_cancelled();

signals:
  void reload_theme();
  void reload_character();
  void reload_audiotracks();
  void server_status_changed(ServerStatus);

private:
  AOConfig *ao_config = nullptr;
  AOConfigPanel *ao_config_panel = nullptr;
  DRDiscord *dr_discord = nullptr;

  DRServerSocket *m_server_socket = nullptr;
  ServerStatus m_server_status = NotConnected;

  Lobby *m_lobby = nullptr;
  bool is_lobby_constructed = false;

  bool is_courtroom_constructed = false;

  ///////////////server metadata////////////////

  VersionNumber m_server_client_version;
  VersionStatus m_server_client_version_status = VersionStatus::Ok;

  ///////////////loading info///////////////////
  // player number, it's hardly used but might be needed for some old servers
  int m_client_id = 0;

  QString m_server_software;

  int m_character_count = 0;
  int m_loaded_characters = 0;
  int m_evidence_count = 0;
  int m_loaded_evidence = 0;
  int m_music_count = 0;
  int m_loaded_music = 0;
  bool m_loaded_music_list = false;
  bool m_loaded_area_list = false;

private slots:
  void _p_handle_server_state_update(DRServerSocket::ConnectionState);
  void _p_handle_server_packet(DRPacket);
  void on_courtroom_closing();
  void on_courtroom_destroyed();
  void resolve_current_theme();
  void handle_theme_modification();
  void handle_character_reloading();
  void handle_audiotracks_reloading();

public:
  QString get_sfx_dir_path();
  QString get_sfx_path(QString sfx);
  QString get_sfx_noext_path(QString p_file);
  QString get_ambient_sfx_path(QString p_file);
  QString get_character_sprite_path(QString character, QString emote, QString prefix, bool use_placeholder);
  QString get_character_sprite_pre_path(QString character, QString emote);
  QString get_character_sprite_idle_path(QString character, QString emote);
  QString get_character_sprite_talk_path(QString character, QString emote);
  QString get_background_sprite_path(QString background, QString image);
  QString get_background_sprite_noext_path(QString background, QString image);
  QString get_shout_sprite_path(QString character, QString shout);
  QString get_theme_sprite_path(QString file_name, QString character);
  QString get_theme_sprite_path(QString file_name);
  QString get_effect_anim_path(QString file_name);
  QString get_wtce_anim_path(QString file_name);

public:
  bool notify(QObject *receiver, QEvent *event) override;
  void setInstance(AOApplication *newInstance);
  QString getCurrentGamemode();
  QString getCurrentTime();
};

#endif // AOAPPLICATION_H
