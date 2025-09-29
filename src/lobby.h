#ifndef LOBBY_H
#define LOBBY_H

#include "engine/interface/scenes/replay_window.h"
#include "engine/interface/widgets/scene_widget.h"
#include "engine/interface/widgets/workshop_list.h"
#include "engine/interface/scenes/downloader_prompt.h"

class AOApplication;
class RPButton;
class AOConfig;
class AOImageDisplay;
class DRChatLog;
class DRMasterClient;
class RPTextEdit;

class QListWidget;
class QLineEdit;
class QProgressBar;
class QTextBrowser;

class Lobby : public SceneWidget
{
  Q_OBJECT

public:
  Lobby(AOApplication *p_ao_app);
  ~Lobby();

  DRServerInfoList get_combined_server_list();

  void set_choose_a_server();
  void set_player_count(int players_online, int max_players);
  void set_loading_text(QString p_text);
  void set_stylesheet(QWidget *widget, QString target_tag);
  void set_stylesheets();
  void set_fonts();
  void show_loading_overlay();
  void hide_loading_overlay();
  DRServerInfo get_selected_server();
  void set_loading_value(int p_value);

signals:
  void server_list_changed();
  void favorite_server_list_changed();
  void combined_server_list_changed();

private:
  QString m_currentCategory = "";
  QString m_currentPackage = "";

  AOApplication *ao_app = nullptr;
  AOConfig *ao_config = nullptr;

  DRMasterClient *m_master_client = nullptr;
  DRServerInfoList m_server_list;
  DRServerInfoList m_favorite_server_list;
  DRServerInfoList m_combined_server_list;
  DRServerInfo m_current_server;

  // ui
  AOImageDisplay *ui_background = nullptr;
  RPButton *ui_public_server_filter = nullptr;
  RPButton *ui_favorite_server_filter = nullptr;
  enum ServerFilter
  {
    NoFilter,
    PublicOnly,
    FavoriteOnly,
  };
  ServerFilter m_server_filter = NoFilter;

  AOImageDisplay *ui_gallery_background = nullptr;
  AOImageDisplay *ui_gallery_preview = nullptr;
  RPButton *ui_gallery_play = nullptr;
  RPButton *ui_gallery_toggle = nullptr;

  RPButton *ui_workshop_download = nullptr;
  RPButton *ui_workshop_upload = nullptr;

  AOImageDisplay *ui_workshop_background = nullptr;
  AOImageDisplay *ui_workshop_preview = nullptr;
  RPButton *ui_workshop_toggle = nullptr;

  QListWidget *ui_replay_list = nullptr;
  QComboBox *ui_gallery_packages = nullptr;
  QComboBox *ui_gallery_categories = nullptr;

  RPButton *ui_refresh = nullptr;
  RPButton *ui_toggle_favorite = nullptr;
  RPButton *ui_connect = nullptr;

  RPTextEdit *ui_version = nullptr;
  RPButton *ui_config_panel = nullptr;
  QListWidget *ui_server_list = nullptr;
  RPTextEdit *ui_player_count = nullptr;
  QTextBrowser *ui_description = nullptr;
  DRChatLog *ui_chatbox = nullptr;
  AOImageDisplay *ui_loading_background = nullptr;
  RPTextEdit *ui_loading_text = nullptr;
  QProgressBar *ui_progress_bar = nullptr;
  RPButton *ui_cancel = nullptr;
  WorkshopListWidget *workshop_list = nullptr;
  QTextBrowser *ui_workshop_description = nullptr;


  QString m_currentBrowserUrl = "";
  QString m_currentWorkshopCharacter = "";
  int m_currentWorkshopId = 0;
  QNetworkAccessManager *workshopPreviewDownloader;

  QMenu *ui_server_menu;
  std::optional<int> m_server_index;
  enum ServerType
  {
    NoServerType,
    FavoriteServer,
  };
  ServerType m_server_index_type = NoServerType;
  QAction *ui_create_server;
  QAction *ui_modify_server;
  QAction *ui_delete_server;
  QMenu *ui_swap_with;
  QAction *ui_move_up_server;
  QAction *ui_move_down_server;

  ReplayWindow *m_replayWindow = nullptr;

  void load_settings();
  void save_settings();

  void load_favorite_server_list();
  void load_legacy_favorite_server_list();
  void save_favorite_server_list();

private slots:
  void update_widgets();

  void request_advertiser_update();
  void update_motd();
  void update_server_list();
  void set_favorite_server_list(DRServerInfoList server_list);
  void update_combined_server_list();
  void toggle_public_server_filter();
  void toggle_favorite_server_filter();
  void update_server_filter_buttons();
  void update_server_listing();
  void filter_server_listing();
  void select_current_server();

  void onReplayRowChanged(int row);
  void onGalleryPackageChanged(int index);
  void onGalleryCategoryChanged(int index);
  void onGalleryToggle();
  void onWorkshopToggle();
  void onGalleryPlay();
  void onWorkshopBrowser();
  void onWorkshopUpload();
  void on_refresh_released();
  void on_add_to_fav_released();
  void on_connect_released();
  void on_config_pressed();
  void on_config_released();
  void connect_to_server(int row);

  void show_server_context_menu(QPoint);
  void prompt_server_info_editor();
  void create_server_info();
  void modify_server_info();
  void prompt_delete_server();
  void move_up_server();
  void move_down_server();

  void _p_update_description();
};

#endif // LOBBY_H
