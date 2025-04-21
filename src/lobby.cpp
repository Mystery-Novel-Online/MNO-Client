#include "lobby.h"

#include "aoapplication.h"
#include "aobutton.h"
#include "aoconfig.h"
#include "aoimagedisplay.h"
#include "commondefs.h"
#include "datatypes.h"
#include "modules/managers/localization_manager.h"
#include "modules/managers/emotion_manager.h"
#include "debug_functions.h"
#include "drchatlog.h"
#include "drmasterclient.h"
#include "drpacket.h"
#include "drserverinfoeditor.h"
#include "drtextedit.h"
#include "drtheme.h"
#include "modules/managers/scene_manager.h"
#include "theme.h"
#include "version.h"

#include <QAction>
#include <QCollator>
#include <QDebug>
#include <QFile>
#include <QFontDatabase>
#include <QIcon>
#include <QInputDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>
#include <QProgressBar>
#include <QScopedPointer>
#include <QSettings>

#include <utility>

Lobby::Lobby(AOApplication *p_ao_app)
    : QMainWindow()
{
  ao_app = p_ao_app;
  ao_config = new AOConfig(this);
  m_master_client = new DRMasterClient(this);

  setWindowTitle("Danganronpa Online (" + get_version_string() + ")");

  ui_background = new AOImageDisplay(this, ao_app);
  ui_public_server_filter = new AOButton(this, ao_app);
  ui_favorite_server_filter = new AOButton(this, ao_app);
  ui_refresh = new AOButton(this, ao_app);
  ui_toggle_favorite = new AOButton(this, ao_app);
  ui_connect = new AOButton(this, ao_app);
  ui_version = new DRTextEdit(this);
  ui_version->setFrameStyle(QFrame::NoFrame);
  ui_version->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_version->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_version->setReadOnly(true);
  ui_config_panel = new AOButton(this, ao_app);
  ui_server_list = new QListWidget(this);
  ui_server_list->setContextMenuPolicy(Qt::CustomContextMenu);

  ui_server_menu = new QMenu(this);
  ui_server_menu->addSection(tr("Server"));
  ui_create_server = ui_server_menu->addAction(LocalizationManager::get().getLocalizationText("SERVER_FAVORITES_ADD"));
  ui_modify_server = ui_server_menu->addAction(LocalizationManager::get().getLocalizationText("SERVER_FAVORITES_EDIT"));
  ui_move_up_server = ui_server_menu->addAction(LocalizationManager::get().getLocalizationText("SERVER_FAVORITES_UP"));
  ui_move_down_server = ui_server_menu->addAction(LocalizationManager::get().getLocalizationText("SERVER_FAVORITES_DOWN"));
  ui_delete_server = ui_server_menu->addAction(LocalizationManager::get().getLocalizationText("SERVER_FAVORITES_REMOVE"));

  ui_player_count = new DRTextEdit(this);
  ui_player_count->setFrameStyle(QFrame::NoFrame);
  ui_player_count->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_player_count->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui_player_count->setWordWrapMode(QTextOption::NoWrap);
  ui_player_count->setReadOnly(true);
  ui_description = new QTextBrowser(this);
  ui_description->setOpenExternalLinks(true);
  ui_description->setReadOnly(true);
  ui_chatbox = new DRChatLog(this);
  ui_chatbox->setOpenExternalLinks(true);
  ui_chatbox->setReadOnly(true);
  ui_loading_background = new AOImageDisplay(this, ao_app);
  ui_loading_text = new DRTextEdit(ui_loading_background);
  ui_progress_bar = new QProgressBar(ui_loading_background);
  ui_progress_bar->setMinimum(0);
  ui_progress_bar->setMaximum(100);
  ui_progress_bar->setStyleSheet("QProgressBar{ color: white; }");
  ui_cancel = new AOButton(ui_loading_background, ao_app);

  connect(ao_app, SIGNAL(reload_theme()), this, SLOT(update_widgets()));
  connect(ao_app, &AOApplication::server_status_changed, this, &Lobby::_p_update_description);

  connect(ao_config, SIGNAL(theme_changed(QString)), this, SLOT(update_widgets()));
  connect(ao_config, SIGNAL(server_advertiser_changed(QString)), m_master_client, SLOT(set_address(QString)));

  connect(m_master_client, SIGNAL(address_changed()), this, SLOT(request_advertiser_update()));
  connect(m_master_client, SIGNAL(motd_changed()), this, SLOT(update_motd()));
  connect(m_master_client, SIGNAL(server_list_changed()), this, SLOT(update_server_list()));

  connect(ui_public_server_filter, SIGNAL(clicked()), this, SLOT(toggle_public_server_filter()));

  connect(ui_favorite_server_filter, SIGNAL(clicked()), this, SLOT(toggle_favorite_server_filter()));

  connect(ui_refresh, SIGNAL(pressed()), this, SLOT(on_refresh_pressed()));
  connect(ui_refresh, SIGNAL(released()), this, SLOT(on_refresh_released()));

  connect(ui_toggle_favorite, SIGNAL(pressed()), this, SLOT(on_add_to_fav_pressed()));
  connect(ui_toggle_favorite, SIGNAL(released()), this, SLOT(on_add_to_fav_released()));

  connect(ui_connect, SIGNAL(pressed()), this, SLOT(on_connect_pressed()));
  connect(ui_connect, SIGNAL(released()), this, SLOT(on_connect_released()));

  connect(ui_config_panel, SIGNAL(pressed()), this, SLOT(on_config_pressed()));
  connect(ui_config_panel, SIGNAL(released()), this, SLOT(on_config_released()));

  connect(ui_server_list, SIGNAL(currentRowChanged(int)), this, SLOT(connect_to_server(int)));
  connect(ui_server_list, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(show_server_context_menu(QPoint)));

  connect(ui_create_server, SIGNAL(triggered(bool)), this, SLOT(create_server_info()));
  connect(ui_modify_server, SIGNAL(triggered(bool)), this, SLOT(modify_server_info()));
  connect(ui_delete_server, SIGNAL(triggered(bool)), this, SLOT(prompt_delete_server()));
  connect(ui_move_up_server, SIGNAL(triggered(bool)), this, SLOT(move_up_server()));
  connect(ui_move_down_server, SIGNAL(triggered(bool)), this, SLOT(move_down_server()));

  connect(ui_cancel, SIGNAL(clicked()), ao_app, SLOT(loading_cancelled()));

  load_settings();
  load_favorite_server_list();
  update_widgets();
  m_master_client->set_address(ao_config->server_advertiser());
  set_choose_a_server();

  EmotionManager::get().wEmoteList = {};
}

Lobby::~Lobby()
{
  save_settings();
}

DRServerInfoList Lobby::get_combined_server_list()
{
  return m_combined_server_list;
}

// sets images, position and size
void Lobby::update_widgets()
{
  ao_app->current_theme->InitTheme();

  pos_size_type f_lobby = ao_app->get_element_dimensions("lobby", LOBBY_DESIGN_INI);
  if (f_lobby.width < 0 || f_lobby.height < 0)
  {
    qWarning() << "W: did not find lobby width or height in " << LOBBY_DESIGN_INI;
    f_lobby.width = 517;
    f_lobby.height = 666;
  }
  setWindowState(Qt::WindowNoState);
  resize(f_lobby.width, f_lobby.height);
  center_widget_to_screen(this);

  set_size_and_pos(ui_background, "lobby", LOBBY_DESIGN_INI, ao_app);
  ui_background->set_theme_image("lobbybackground.png");

  set_size_and_pos(ui_public_server_filter, "public_servers", LOBBY_DESIGN_INI, ao_app);
  ui_public_server_filter->set_image(m_server_filter == PublicOnly ? "publicservers_selected.png" : "publicservers.png");

  set_size_and_pos(ui_favorite_server_filter, "favorites", LOBBY_DESIGN_INI, ao_app);
  ui_favorite_server_filter->set_image(m_server_filter == FavoriteOnly ? "favorites_selected.png" : "favorites.png");

  set_size_and_pos(ui_refresh, "refresh", LOBBY_DESIGN_INI, ao_app);
  ui_refresh->set_image("refresh.png");

  set_size_and_pos(ui_toggle_favorite, "add_to_fav", LOBBY_DESIGN_INI, ao_app);
  ui_toggle_favorite->set_image("addtofav.png");

  set_size_and_pos(ui_connect, "connect", LOBBY_DESIGN_INI, ao_app);
  ui_connect->set_image("connect.png");

  set_size_and_pos(ui_version, "version", LOBBY_DESIGN_INI, ao_app);
  ui_version->setText("Version: " + get_version_string());

  set_size_and_pos(ui_config_panel, "config_panel", LOBBY_DESIGN_INI, ao_app);
  ui_config_panel->set_image_and_text("lobby_config_panel.png", "Config");
  if (ui_config_panel->isHidden() || ui_config_panel->size().isEmpty())
  {
    ui_config_panel->resize(64, 64);
    ui_config_panel->move(0, 0);
    ui_config_panel->show();
  }

  set_size_and_pos(ui_server_list, "server_list", LOBBY_DESIGN_INI, ao_app);
  ui_server_list->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "font: bold;");

  set_size_and_pos(ui_player_count, "player_count", LOBBY_DESIGN_INI, ao_app);
  ui_player_count->setStyleSheet("font: bold;"
                                 "color: white;"
                                 "qproperty-alignment: AlignCenter;");

  set_size_and_pos(ui_description, "description", LOBBY_DESIGN_INI, ao_app);
  ui_description->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                "color: white;");

  set_size_and_pos(ui_chatbox, "chatbox", LOBBY_DESIGN_INI, ao_app);
  ui_chatbox->setReadOnly(true);
  ui_chatbox->setStyleSheet("QTextBrowser{background-color: rgba(0, 0, 0, 0);}");

  ui_loading_background->resize(this->width(), this->height());
  ui_loading_background->set_theme_image("loadingbackground.png");

  set_size_and_pos(ui_loading_text, "loading_label", LOBBY_DESIGN_INI, ao_app);
  ui_loading_text->setFont(QFont("Arial", 20, QFont::Bold));
  ui_loading_text->setReadOnly(true);
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->setFrameStyle(QFrame::NoFrame);
  ui_loading_text->setStyleSheet("background-color: rgba(0, 0, 0, 0);"
                                 "color: rgba(255, 128, 0, 255);");
  ui_loading_text->append("Loading");

  set_size_and_pos(ui_progress_bar, "progress_bar", LOBBY_DESIGN_INI, ao_app);
  set_size_and_pos(ui_cancel, "cancel", LOBBY_DESIGN_INI, ao_app);
  ui_cancel->setText("Cancel");

  ui_loading_background->hide();

  set_fonts();
  set_stylesheets();
  update_server_listing();

  update_server_filter_buttons();
}

void Lobby::set_fonts()
{
  set_drtextedit_font(ui_player_count, "player_count", LOBBY_FONTS_INI, ao_app);
  set_font(ui_description, "description", LOBBY_FONTS_INI, ao_app);
  set_font(ui_chatbox, "chatbox", LOBBY_FONTS_INI, ao_app);
  set_drtextedit_font(ui_loading_text, "loading_text", LOBBY_FONTS_INI, ao_app);
  set_font(ui_server_list, "server_list", LOBBY_FONTS_INI, ao_app);
}

void Lobby::set_stylesheet(QWidget *widget, QString target_tag)
{
  QString f_file = "lobby_stylesheets.css";
  QString style_sheet_string = ao_app->get_stylesheet(target_tag, f_file);
  if (style_sheet_string != "")
    widget->setStyleSheet(style_sheet_string);
}

void Lobby::set_stylesheets()
{
  set_stylesheet(ui_player_count, "[PLAYER COUNT]");
  set_stylesheet(ui_description, "[DESCRIPTION]");
  set_stylesheet(ui_chatbox, "[CHAT BOX]");
  set_stylesheet(ui_loading_text, "[LOADING TEXT]");
  set_stylesheet(ui_server_list, "[SERVER LIST]");
}

void Lobby::show_loading_overlay()
{
  ui_loading_background->show();
}

void Lobby::hide_loading_overlay()
{
  ui_loading_background->hide();
}

void Lobby::set_loading_text(QString p_text)
{
  ui_loading_text->clear();
  ui_loading_text->setAlignment(Qt::AlignCenter);
  ui_loading_text->append(p_text);
}

DRServerInfo Lobby::get_selected_server()
{
  return m_current_server;
}

void Lobby::set_loading_value(int p_value)
{
  ui_progress_bar->setValue(p_value);
}

void Lobby::load_settings()
{
  QSettings l_ini(ao_app->get_base_file_path(BASE_SERVER_BROWSER_INI), QSettings::IniFormat);
  l_ini.setIniCodec("UTF-8");

  l_ini.beginGroup("filters");
  m_server_filter = ServerFilter(l_ini.value("server_filter", NoFilter).toInt());
  l_ini.endGroup();
}

void Lobby::save_settings()
{
  QSettings l_ini(ao_app->get_base_file_path(BASE_SERVER_BROWSER_INI), QSettings::IniFormat);
  l_ini.setIniCodec("UTF-8");

  l_ini.beginGroup("filters");
  l_ini.setValue("server_filter", int(m_server_filter));
  l_ini.endGroup();
  l_ini.sync();
}

void Lobby::load_favorite_server_list()
{
  const QString l_file_path = ao_app->find_asset_path(ao_app->get_base_file_path(BASE_FAVORITE_SERVERS_INI));
  if (l_file_path.isEmpty())
  {
    load_legacy_favorite_server_list();
    return;
  }

  DRServerInfoList l_server_list;
  QSettings l_ini(l_file_path, QSettings::IniFormat);
  l_ini.setIniCodec("UTF-8");
  l_server_list.clear();
  QStringList l_group_list = l_ini.childGroups();

  {
    QCollator l_sorter;
    l_sorter.setNumericMode(true);
    std::sort(l_group_list.begin(), l_group_list.end(), l_sorter);
  }

  for (const QString &i_group : qAsConst(l_group_list))
  {
    l_ini.beginGroup(i_group);
    DRServerInfo l_server;
    l_server.name = l_ini.value("name").toString();
    l_server.description = l_ini.value("description").toString();
    l_server.address = l_ini.value("address").toString();
    l_server.port = l_ini.value("port").toInt();
    l_server_list.append(std::move(l_server));
    l_ini.endGroup();
  }
  set_favorite_server_list(l_server_list);
}

void Lobby::load_legacy_favorite_server_list()
{
  DRServerInfoList l_server_list;
  QFile l_file(ao_app->get_base_file_path(BASE_SERVERLIST_TXT));
  if (l_file.open(QIODevice::ReadOnly))
  {
    QTextStream in(&l_file);
    while (!in.atEnd())
    {
      const QStringList l_contents = in.readLine().split(":");
      if (l_contents.length() < 3)
        continue;
      DRServerInfo f_server;
      f_server.address = l_contents.at(0);
      f_server.port = l_contents.at(1).toInt();
      f_server.name = l_contents.at(2);
      l_server_list.append(std::move(f_server));
    }
    l_file.remove();
  }
  set_favorite_server_list(l_server_list);
}

void Lobby::save_favorite_server_list()
{
  QSettings l_ini(ao_app->get_base_file_path(BASE_FAVORITE_SERVERS_INI), QSettings::IniFormat);
  l_ini.setIniCodec("UTF-8");

  l_ini.clear();
  for (int i = 0; i < m_favorite_server_list.length(); ++i)
  {
    const DRServerInfo &i_server = m_favorite_server_list.at(i);
    l_ini.beginGroup(QString::number(i));
    l_ini.setValue("name", i_server.name);
    l_ini.setValue("description", i_server.description);
    l_ini.setValue("address", i_server.address);
    l_ini.setValue("port", i_server.port);
    l_ini.endGroup();
  }
  l_ini.sync();
}

void Lobby::request_advertiser_update()
{
  m_master_client->request_motd();
  m_master_client->request_server_list();
}

void Lobby::update_motd()
{
  ui_chatbox->append_html(m_master_client->motd());
}

void Lobby::update_server_list()
{
  m_server_list = m_master_client->server_list();
  update_combined_server_list();
  emit server_list_changed();
}

void Lobby::set_favorite_server_list(DRServerInfoList p_server_list)
{
  m_favorite_server_list = p_server_list;
  save_favorite_server_list();
  update_combined_server_list();
  emit favorite_server_list_changed();
}

void Lobby::update_combined_server_list()
{
  m_combined_server_list = m_favorite_server_list + m_server_list;
  update_server_listing();
}

void Lobby::update_server_listing()
{
  ui_server_list->clear();
  const QIcon l_favorite_icon = QPixmap(ao_app->find_theme_asset_path("favorite_server.png"));
  const QBrush l_favorite_color = ao_app->get_color("favorite_server_color", LOBBY_DESIGN_INI);
  for (int i = 0; i < m_combined_server_list.length(); ++i)
  {
    const DRServerInfo &l_server = m_combined_server_list.at(i);
    QListWidgetItem *l_server_item = new QListWidgetItem;
    ui_server_list->addItem(l_server_item);
    l_server_item->setText(l_server.name);
    l_server_item->setData(Qt::UserRole, false);
    if (i < m_favorite_server_list.length())
    {
      l_server_item->setIcon(l_favorite_icon);
      l_server_item->setBackground(l_favorite_color);
      l_server_item->setData(Qt::UserRole, true);
    }
  }
  filter_server_listing();
}

void Lobby::filter_server_listing()
{
  for (int i = 0; i < ui_server_list->count(); ++i)
  {
    QListWidgetItem *l_server_item = ui_server_list->item(i);
    l_server_item->setHidden(m_server_filter == (l_server_item->data(Qt::UserRole).toBool() ? PublicOnly : FavoriteOnly));
  }
  select_current_server();
}

void Lobby::select_current_server()
{
  if (m_current_server.name.isEmpty())
  {
    return;
  }

  for (int i = 0; i < ui_server_list->count(); ++i)
  {
    QListWidgetItem *l_item = ui_server_list->item(i);
    if (l_item->text() == m_current_server.name)
    {
      ui_server_list->scrollToItem(l_item);
      ui_server_list->setCurrentItem(l_item);
      ui_server_list->setFocus();
      break;
    }
  }
}

void Lobby::toggle_public_server_filter()
{
  m_server_filter = m_server_filter == PublicOnly ? NoFilter : PublicOnly;
  update_server_filter_buttons();
}

void Lobby::toggle_favorite_server_filter()
{
  m_server_filter = m_server_filter == FavoriteOnly ? NoFilter : FavoriteOnly;
  update_server_filter_buttons();
}

void Lobby::update_server_filter_buttons()
{
  ui_public_server_filter->set_image(m_server_filter == PublicOnly ? "publicservers_selected.png" : "publicservers.png");
  ui_favorite_server_filter->set_image(m_server_filter == FavoriteOnly ? "favorites_selected.png" : "favorites.png");
  filter_server_listing();
}

void Lobby::on_refresh_pressed()
{
  ui_refresh->set_image("refresh_pressed.png");
}

void Lobby::on_refresh_released()
{
  ui_refresh->set_image("refresh.png");
  m_master_client->request_server_list();
  load_favorite_server_list();
}

void Lobby::on_add_to_fav_pressed()
{
  ui_toggle_favorite->set_image("addtofav_pressed.png");
}

void Lobby::on_add_to_fav_released()
{
  ui_toggle_favorite->set_image("addtofav.png");
  const auto l_index = ui_server_list->currentIndex();
  if (!l_index.isValid() || l_index.row() < m_favorite_server_list.length())
  {
    return;
  }
  const auto l_selected_server = m_combined_server_list.at(l_index.row());
  DRServerInfoList l_server_list = m_favorite_server_list;
  if (m_favorite_server_list.contains(l_selected_server))
  {
    l_server_list.removeAll(m_current_server);
  }
  else
  {
    l_server_list.append(m_current_server);
  }
  set_favorite_server_list(l_server_list);
}

void Lobby::on_connect_pressed()
{
  SceneManager::get().clearPlayerDataList();
  ui_connect->set_image("connect_pressed.png");
}

void Lobby::on_connect_released()
{
  const VersionStatus l_status = ao_app->get_server_client_version_status();
  if (l_status != VersionStatus::Ok)
  {
    QString l_reason;
    switch (l_status)
    {
    case VersionStatus::NotCompatible:
      l_reason = "The server did not report any client version.";
      break;
    case VersionStatus::ServerOutdated:
      l_reason = QString("The server is outdated.<br />(Server version: <b>%1</b>, expected version: <b>%2</b>)").arg(ao_app->get_server_client_version().to_string(), get_version_number().to_string());
      break;
    case VersionStatus::ClientOutdated:
      l_reason = QString("Your client is outdated.<br />(Client version: <b>%1</b>, expected version: <b>%2</b>)").arg(get_version_number().to_string(), ao_app->get_server_client_version().to_string());
      break;
    default:
      break;
    }

    call_warning("You are connecting to an <b>incompatible</b> DRO server.<br /><br />Reason: " + l_reason +
                 "<br /><br />"
                 "The client may not work properly, if at all.");
  }

  ui_connect->set_image("connect.png");
  ao_app->send_server_packet(DRPacket("askchaa"));
}

void Lobby::on_config_pressed()
{
  ui_config_panel->set_image("lobby_config_panel_pressed.png");
}

void Lobby::on_config_released()
{
  ui_config_panel->set_image("lobby_config_panel.png");
  ao_app->toggle_config_panel();
}

void Lobby::connect_to_server(int p_row)
{
  if (p_row == -1)
    return;

  const DRServerInfo l_prev_server = std::move(m_current_server);
  m_current_server = m_combined_server_list.at(p_row);
  if (l_prev_server != m_current_server)
  {
    ui_player_count->setText(nullptr);
    ao_app->connect_to_server(m_current_server);
  }
}

void Lobby::show_server_context_menu(QPoint p_point)
{
  const QPoint l_global_point = ui_server_list->viewport()->mapToGlobal(p_point);

  m_server_index.reset();
  m_server_index_type = NoServerType;
  const auto l_item = ui_server_list->indexAt(p_point);
  ui_create_server->setEnabled(true);
  ui_modify_server->setDisabled(true);
  ui_delete_server->setDisabled(true);
  ui_move_up_server->setDisabled(true);
  ui_move_down_server->setDisabled(true);
  if (l_item.isValid())
  {
    const int l_item_row = l_item.row();
    m_server_index = l_item_row;
    if (l_item_row < m_favorite_server_list.length())
    {
      m_server_index_type = FavoriteServer;
      ui_modify_server->setEnabled(true);
      ui_delete_server->setEnabled(true);
      ui_move_up_server->setEnabled(l_item_row - 1 >= 0);
      ui_move_down_server->setEnabled(l_item_row + 1 < m_favorite_server_list.length());
    }
  }
  ui_server_menu->popup(l_global_point);
}

void Lobby::prompt_server_info_editor()
{
  DRServerInfoEditor l_editor(this);
  l_editor.setWindowTitle(tr("Server Info Editor"));
  if (m_server_index.has_value())
  {
    l_editor.set_server_info(m_combined_server_list.at(m_server_index.value()));
  }
  if (l_editor.exec())
  {
    auto l_server_info = l_editor.get_server_info();
    auto l_server_info_list = m_favorite_server_list;
    if (m_server_index.has_value() && m_server_index.value() < l_server_info_list.length())
    {
      l_server_info_list.replace(m_server_index.value(), l_server_info);
    }
    else
    {
      l_server_info_list.append(l_server_info);
    }
    set_favorite_server_list(l_server_info_list);
  }
}

void Lobby::create_server_info()
{
  if (m_server_index_type == FavoriteServer)
  {
    m_server_index.reset();
  }
  prompt_server_info_editor();
}

void Lobby::modify_server_info()
{
  prompt_server_info_editor();
}

void Lobby::prompt_delete_server()
{
  const auto l_server_index = m_server_index.value();
  const auto l_server = m_combined_server_list.at(l_server_index);
  if (prompt_warning(tr("Are you sure you wish to remove the server %1?").arg(l_server.name)))
  {
    auto l_server_list = m_favorite_server_list;
    l_server_list.remove(l_server_index);
    set_favorite_server_list(l_server_list);
  }
}

void Lobby::move_up_server()
{
  auto l_server_list = m_favorite_server_list;
  const int l_server_index = m_server_index.value();
  std::swap(l_server_list.begin()[l_server_index], l_server_list.begin()[l_server_index - 1]);
  set_favorite_server_list(l_server_list);
}

void Lobby::move_down_server()
{
  auto l_server_list = m_favorite_server_list;
  const int l_server_index = m_server_index.value();
  std::swap(l_server_list.begin()[l_server_index], l_server_list.begin()[l_server_index + 1]);
  set_favorite_server_list(l_server_list);
}

void Lobby::_p_update_description()
{
  QMap<AOApplication::ServerStatus, QString> l_report_map{
      {AOApplication::NotConnected, LocalizationManager::get().getLocalizationText("CONNECTION_NOT")},
      {AOApplication::Connecting, LocalizationManager::get().getLocalizationText("CONNECTION_CONNECTING")},
      {AOApplication::Connected, LocalizationManager::get().getLocalizationText("CONNECTION_CONNECTED")},
      {AOApplication::Joined, LocalizationManager::get().getLocalizationText("CONNECTION_JOINED")},
      {AOApplication::TimedOut, LocalizationManager::get().getLocalizationText("CONNECTION_TIMEDOUT")},
      {AOApplication::Disconnected, LocalizationManager::get().getLocalizationText("CONNECTION_NOT")},
  };

  QString l_message = l_report_map[ao_app->last_server_status()];

  if (!m_current_server.name.isEmpty())
  {
    l_message = QString("%1\n\n"
                        "==== STATUS ====\n"
                        "%2")
                    .arg(m_current_server.name.toHtmlEscaped())
                    .arg(l_message);
  }

  if (!m_current_server.description.isEmpty())
  {
    QString l_description = m_current_server.description.toHtmlEscaped();
    const QRegExp l_regex("(https?://[^\\s/$.?#].[^\\s]*)");
    if (l_description.contains(l_regex))
    {
      l_description.replace(l_regex, "<a href=\"\\1\">\\1</a>");
    }
    l_message = QString("==== DESCRIPTION ====\n"
                        "%1")
                    .arg(l_description);
  }

  ui_description->setHtml(l_message.replace("\n", "<br />"));
}

void Lobby::set_choose_a_server()
{
  ui_player_count->setText(nullptr);
  _p_update_description();
}

void Lobby::set_player_count(int players_online, int max_players)
{
  const QString f_string = "Connected: " + QString::number(players_online) + "/" + QString::number(max_players);
  ui_player_count->setText(f_string);
  ui_player_count->setAlignment(Qt::AlignHCenter);
}
