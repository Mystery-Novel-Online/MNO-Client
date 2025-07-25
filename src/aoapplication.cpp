#include "pch.h"

#include "aoconfig.h"
#include "aoconfigpanel.h"
#include "debug_functions.h"
#include "drdiscord.h"
#include "drserversocket.h"
#include "lobby.h"
#include "theme.h"
#include "drtheme.h"
#include "version.h"

#include "engine/system/localization.h"
#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_writing.h"
#include "engine/fs/fs_characters.h"
#include "engine/system/runtime_loop.h"
#include "engine/system/effects.h"
#include "engine/system/audio.h"
#include "engine/network/metadata/server_metadata.h"

AOApplication *AOApplication::m_Instance = nullptr;

void AOApplication::setInstance(AOApplication *newInstance)
{
  m_Instance = newInstance;
}

AOApplication *AOApplication::getInstance()
{
  return m_Instance;
}

AOApplication::AOApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{
  ao_config = new AOConfig(this);

  DirUtils::CreateInitialFolders();

  SceneManager::get().pConfigAO = ao_config;
  engine::system::localization::initialize();

  ao_config_panel = new AOConfigPanel(this);

  dr_discord = new DRDiscord(this);

  m_server_socket = new DRServerSocket(this);
  setInstance(this);
  audio::Initialize();

  connect(ao_config, SIGNAL(theme_changed(QString)), this, SLOT(handle_theme_modification()));
  connect(ao_config, SIGNAL(gamemode_changed(QString)), this, SLOT(handle_theme_modification()));
  connect(ao_config, SIGNAL(timeofday_changed(QString)), this, SLOT(handle_theme_modification()));
  connect(ao_config, SIGNAL(manual_gamemode_changed(QString)), this, SLOT(handle_theme_modification()));
  connect(ao_config, SIGNAL(manual_gamemode_selection_changed(bool)), this, SLOT(handle_theme_modification()));
  connect(ao_config, SIGNAL(manual_timeofday_changed(QString)), this, SLOT(handle_theme_modification()));
  connect(ao_config, SIGNAL(manual_timeofday_selection_changed(bool)), this, SLOT(handle_theme_modification()));
  connect(ao_config_panel, SIGNAL(reload_theme()), this, SLOT(handle_theme_modification()));
  connect(ao_config_panel, SIGNAL(reload_character()), this, SLOT(handle_character_reloading()));
  connect(ao_config_panel, SIGNAL(reload_audiotracks()), this, SLOT(handle_audiotracks_reloading()));
  ao_config_panel->hide();

  dr_discord->set_presence(ao_config->discord_presence());
  dr_discord->set_hide_server(ao_config->discord_hide_server());
  dr_discord->set_hide_character(ao_config->discord_hide_character());
  connect(ao_config, SIGNAL(discord_presence_changed(bool)), dr_discord, SLOT(set_presence(bool)));
  connect(ao_config, SIGNAL(discord_hide_server_changed(bool)), dr_discord, SLOT(set_hide_server(bool)));
  connect(ao_config, SIGNAL(discord_hide_character_changed(bool)), dr_discord, SLOT(set_hide_character(bool)));

  connect(m_server_socket, &DRServerSocket::connection_state_changed, this, &AOApplication::_p_handle_server_state_update);
  connect(m_server_socket, SIGNAL(packet_received(DRPacket)), this, SLOT(_p_handle_server_packet(DRPacket)));

  CharacterRepository::loadFavorites();
  reload_packages();
  resolve_current_theme();

  engine::system::effects::reload();

  QTimer* timer = new QTimer(this);
  connect(timer, &QTimer::timeout, this, [=]() { RuntimeLoop::Update(); });
  timer->start(16);
}

AOApplication::~AOApplication()
{
  qInfo() << "Closing Mystery Novel Online...";
  destruct_lobby();
  destruct_courtroom();
}

void AOApplication::leave_server()
{
  m_server_status = NotConnected;
  m_server_socket->disconnect_from_server();
}

Lobby *AOApplication::get_lobby() const
{
  return m_lobby;
}

void AOApplication::construct_lobby()
{
  if (is_lobby_constructed)
  {
    qDebug() << "W: lobby was attempted constructed when it already exists";
    return;
  }

  m_lobby = new Lobby(this);
  is_lobby_constructed = true;
  center_widget_to_screen(m_lobby);
  m_lobby->show();

  dr_discord->set_state(DRDiscord::State::Idle);
  dr_discord->clear_server_name();
}

void AOApplication::destruct_lobby()
{
  if (!is_lobby_constructed)
  {
    qDebug() << "W: lobby was attempted destructed when it did not exist";
    return;
  }

  delete m_lobby;
  is_lobby_constructed = false;
}

Courtroom *AOApplication::get_courtroom() const
{
  return m_courtroom;
}

void AOApplication::construct_courtroom()
{
  if (is_courtroom_constructed)
  {
    qDebug() << "W: courtroom was attempted constructed when it already exists";
    return;
  }

  m_courtroom = new Courtroom(this);
  connect(m_courtroom, SIGNAL(closing()), this, SLOT(on_courtroom_closing()));
  connect(m_courtroom, SIGNAL(destroyed()), this, SLOT(on_courtroom_destroyed()));
  is_courtroom_constructed = true;
  center_widget_to_screen(m_courtroom);
}

void AOApplication::destruct_courtroom()
{
  // destruct courtroom
  if (is_courtroom_constructed)
  {
    delete m_courtroom;
    m_courtroom = nullptr;
    is_courtroom_constructed = false;
    ao_config->set_gamemode(nullptr);
    ao_config->set_timeofday(nullptr);
  }
  else
  {
    qDebug() << "W: courtroom was attempted destructed when it did not exist";
  }
}

DRDiscord *AOApplication::get_discord() const
{
  return dr_discord;
}

VersionNumber AOApplication::get_server_client_version() const
{
  return m_server_client_version;
}

VersionStatus AOApplication::get_server_client_version_status() const
{
  return m_server_client_version_status;
}

bool AOApplication::is_server_client_version_compatible() const
{
  return m_server_client_version_status == VersionStatus::Ok;
}

void AOApplication::handle_theme_modification()
{
  load_fonts();
  emit reload_theme();
}

void AOApplication::handle_character_reloading()
{
  emit reload_character();
}

void AOApplication::handle_audiotracks_reloading()
{
  emit reload_audiotracks();
}

QString AOApplication::get_sfx_dir_path()
{
  return "sounds/general";
}

QString AOApplication::get_sfx_path(QString p_sfx)
{
  return find_asset_path(FS::Paths::FindFile(get_sfx_dir_path() + "/" + p_sfx));
}

QString AOApplication::get_sfx_noext_path(QString p_file)
{
  QString l_asset_path = nullptr;
  QVector<QString> l_sfx_paths = get_all_package_and_base_paths(get_sfx_dir_path());
  for(QString &l_sfx_path : l_sfx_paths)
  {
    l_asset_path = find_asset_path(l_sfx_path + "/" + p_file, FS::Formats::SupportedAudio());
    if(l_asset_path != nullptr) break;
  }

  return l_asset_path;

}

QString AOApplication::get_ambient_sfx_path(QString p_file)
{
  return find_asset_path(FS::Paths::FindFile("sounds/ambient/" + p_file));
}

QString AOApplication::get_background_sprite_path(QString p_background_name, QString p_image)
{
  const QString l_target_filename = find_asset_path(get_background_path(p_background_name) + "/" + p_image);
  return l_target_filename;
}

QString AOApplication::get_background_sprite_noext_path(QString background, QString image)
{
  return find_asset_path(get_background_path(background) + "/" + image, FS::Formats::SupportedImages());
}

QString AOApplication::getWeatherSprite(QString weather)
{
  return get_case_sensitive_path(FS::Paths::FindFile("animations/weather/" + weather + ".webp"));
}

QString AOApplication::get_shout_sprite_path(QString p_character, QString p_shout, const QString &outfit)
{
  QStringList l_filepath_list{
      fs::characters::getFilePath(p_character, "outfits/" + outfit + "/" + p_shout),
      fs::characters::getFilePath(p_character, "outfits/" + outfit + "/" + p_shout + "_bubble"),
      fs::characters::getFilePath(p_character, p_shout),
      fs::characters::getFilePath(p_character, p_shout + "_bubble"),
  };

  QString l_filename = find_asset_path(l_filepath_list, FS::Formats::AnimatedImages());
  if (l_filename.isEmpty())
  {
    l_filename = find_theme_asset_path(p_shout, FS::Formats::AnimatedImages());
  }

  if (l_filename.isEmpty())
  {
    l_filename = find_asset_path(FS::Paths::FindDirectory("shouts/default/") + p_shout, FS::Formats::AnimatedImages());
  }

  if (l_filename.isEmpty())
  {
    qWarning() << "error: shout not found"
               << "character:" << p_character << "shout:" << p_shout;
  }

  return l_filename;
}

QString AOApplication::get_theme_sprite_path(QString p_file_name, QString p_character)
{
  QString l_file_path;
  if (!p_character.isEmpty())
  {
    QString l_character_file_name = p_file_name;
    if (l_character_file_name == "custom")
    {
      l_character_file_name.append("_bubble");
    }

    QStringList l_path_list{
        fs::characters::getFilePath(p_character, l_character_file_name),
        fs::characters::getFilePath(p_character, "overlay/" + l_character_file_name),
    };
    l_file_path = find_asset_path(l_path_list, FS::Formats::SupportedImages());
  }

  if (l_file_path.isEmpty())
  {
    l_file_path = find_theme_asset_path(p_file_name, FS::Formats::SupportedImages());
    if (l_file_path.isEmpty())
    {
      l_file_path = find_theme_asset_path("placeholder", FS::Formats::SupportedImages());
    }
  }

  return l_file_path;
}

QString AOApplication::get_theme_sprite_path(QString file_name)
{
  return get_theme_sprite_path(file_name, QString{});
}

QString AOApplication::get_effect_anim_path(QString file_name)
{
  QString l_file_path;

  l_file_path = find_theme_asset_path(file_name, FS::Formats::AnimatedImages());

  if (l_file_path.isEmpty())
  {
    l_file_path = find_asset_path(FS::Paths::BasePath() + "effects/default/" + file_name, FS::Formats::AnimatedImages());
  }

  if (l_file_path.isEmpty())
  {
    l_file_path = find_theme_asset_path("placeholder", FS::Formats::AnimatedImages());
  }



  return l_file_path;
}

QString AOApplication::get_wtce_anim_path(QString file_name)
{
  QString l_file_path;

  l_file_path = find_theme_asset_path(file_name, FS::Formats::AnimatedImages());

  if (l_file_path.isEmpty())
  {
    l_file_path = find_asset_path(FS::Paths::BasePath() + "shouts/default/" + file_name, FS::Formats::AnimatedImages());
  }

  if (l_file_path.isEmpty())
  {
    l_file_path = find_theme_asset_path("placeholder", FS::Formats::AnimatedImages());
  }



  return l_file_path;
}

QString AOApplication::get_current_char()
{
  if (!is_courtroom_constructed)
    return nullptr;
  return m_courtroom->get_character_ini();
}

/**
 * @brief Check the path for various known exploits.
 *
 * In order:
 * - Directory traversal (most commonly: "../" jumps)
 * @param p_file The path to check.
 * @return A sanitized path. If any check fails, the path returned is an empty string. The sanitized path does not
 * necessarily exist.
 */
bool AOApplication::is_safe_path(QString p_file)
{
  if (!p_file.contains(".."))
    return true;
  const QStringList l_item_list = p_file.split(QRegularExpression("[\\/]"));
  for (auto it = l_item_list.crbegin(); it != l_item_list.crend(); ++it)
    if (*it == "..")
      return false;
  return true;
}

void AOApplication::toggle_config_panel()
{
  ao_config_panel->setVisible(!ao_config_panel->isVisible());
  if (ao_config_panel->isVisible())
  {
    ao_config_panel->setFocus();
    ao_config_panel->raise();
    center_widget_to_screen(ao_config_panel);
  }
}

bool AOApplication::get_first_person_enabled()
{
  return ao_config->get_bool("first_person", false);
}

void AOApplication::load_fonts()
{
  QFontDatabase l_database;
  QVector<QString> l_font_paths = get_all_package_and_base_paths("fonts");

  for (QString &l_font_path : l_font_paths)
  {
    for (const QFileInfo &fileInfo : QDir(get_case_sensitive_path(l_font_path)).entryInfoList())
      l_database.addApplicationFont(fileInfo.absoluteFilePath());
  }
}

void AOApplication::loading_cancelled()
{
  destruct_courtroom();

  m_lobby->hide_loading_overlay();
}

void AOApplication::on_courtroom_closing()
{
  ao_config_panel->hide();
}

void AOApplication::on_courtroom_destroyed()
{
  ao_config_panel->hide();
}

void AOApplication::resolve_current_theme()
{
  QVector<QString> l_theme_directories = get_all_package_and_base_paths("themes");

  if (l_theme_directories.isEmpty())
  {
    call_warning("It doesn't look like your client is set up correctly. This can be "
                 "due to the following reasons: \n"
                 "1. Check you downloaded and extracted the resources correctly from "
                 "the MNO Discord including the large 'base' folder.\n"
                 "2. If you did, check that the base folder is in the same folder "
                 "where you launched Mystery Novel Online from: " +
                 FS::Paths::ApplicationPath() +
                 "\n"
                 "3. If it is there, check that your current theme folder exists in "
                 "base/themes. ");
  }

  const QString l_current_theme = ao_config->theme();
  std::optional<QString> l_target_theme;


  for (QString &l_theme_dir : l_theme_directories)
  {
    bool l_theme_found = false;
    //Grab file info from directory.
    const QList<QFileInfo> l_info_list = QDir(l_theme_dir).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &i_info : l_info_list)
    {
      const QString l_theme = i_info.fileName();
      if (l_theme == l_current_theme)
      {
        l_target_theme.reset();
        l_theme_found = true;
        break;
      }

      // target theme is always the first
      if (!l_target_theme.has_value())
      {
        l_target_theme = l_theme;
      }
    }

    if (l_theme_found) break;
  }


  if (l_target_theme.has_value())
  {
    if (!ao_config->first_launch())
    {
      call_warning(tr("Your previous theme [%1] is missing; the current theme has been reset.\n\nTo select a different theme, go to the config panel.").arg(l_current_theme));
    }
    ao_config->set_theme(*l_target_theme);
  }

  current_theme = new DRTheme(this);
  current_theme->InitTheme();
}

bool AOApplication::notify(QObject *receiver, QEvent *event)
{
  bool l_done = true;

  try
  {
    l_done = QApplication::notify(receiver, event);
  }
  catch (const std::exception &e)
  {
    qCritical() << "Caught exception:" << e.what();
  }
  catch (...)
  {
    qFatal("Caught UNKNOWN exception. Cannot recover.");
  }

  return l_done;
}
