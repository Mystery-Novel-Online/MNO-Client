#include "pch.h"

#include "modules/theme/thememanager.h"
#include "engine/system/debug/time_debugger.h"
#include "aoconfig.h"
#include "engine/encoding/binary_encoding.h"

#include "debug_functions.h"
#include "engine/system/localization.h"
#include "draudiotrackmetadata.h"
#include "drcharactermovie.h"
#include "drtheme.h"
#include "dreffectmovie.h"
#include "drpacket.h"
#include "drscenemovie.h"
#include "drshoutmovie.h"
#include "drsplashmovie.h"
#include "engine/fs/fs_reading.h"
#include "mk2/graphicsvideoscreen.h"
#include "mk2/spritedynamicreader.h"
#include "mk2/spriteseekingreader.h"
#include "theme.h"
#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_characters.h"
#include "engine/network/metadata/server_metadata.h"
#include "engine/network/metadata/message_metadata.h"
#include "engine/system/theme_scripting.h"
#include "engine/system/text_encoding.h"
#include "engine/system/audio.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/system/replay_playback.h"
#include "engine/system/runtime_loop.h"
#include "engine/system/runtime_values.h"
#include "engine/param/actor_repository.h"
#include "engine/param/actor/actor_loader.h"
#include <engine/discord/workshop_discord.h>
#include <mk2/spritecachingreader.h>
#include <rolechat/actor/JsonActorData.h>
#include "engine/system/config_manager.h"
#include "config_tabs/config_tab_theme.h"

const int Courtroom::DEFAULT_WIDTH = 714;
const int Courtroom::DEFAULT_HEIGHT = 668;

using namespace engine::system;
using namespace engine;

Courtroom::Courtroom(AOApplication *p_ao_app, QWidget *parent)
    : SceneWidget(ThemeSceneType::SceneType_Courtroom, parent)
{
  ao_app = p_ao_app;
  ao_config = new AOConfig(this);
  RuntimeLoop::setWindowFocus(true);

  m_preloader_sync = new mk2::SpriteReaderSynchronizer(this);
  m_preloader_sync->set_threshold(ao_config->caching_threshold());

  //New Config
  ConfigTabTheme *tab = ConfigManager::retrieveTab<ConfigTabTheme>("Theme");
  if(tab)
  {
    connect(tab, &ConfigTabTheme::reloadTheme, this, &Courtroom::reload_theme);
  }

  //Legacy Config
  connect(ao_app, SIGNAL(reload_theme()), this, SLOT(reload_theme()));
  connect(ao_app, SIGNAL(reload_character()), this, SLOT(load_character()));
  connect(ao_app, SIGNAL(reload_audiotracks()), this, SLOT(load_audiotracks()));
  ThemeManager::get().toggleReload();

  create_widgets();
  connect_widgets();

  while (m_chatmessage.length() < OPTIMAL_MESSAGE_SIZE)
  {
    m_chatmessage.append(QString{});
  }
  m_chatmessage[CMPosition] = "wit";

  setup_courtroom();
  map_viewport_viewers();
  map_viewport_readers();
  if (ao_app->current_theme->read_config_bool("use_toggles"))
    ThemeManager::get().resetSelectedTabs();

  set_char_select();
  load_audiotracks();
  reset_viewport();
  user::partner::setPartner(-1);
  ui_slider_horizontal_axis->setValue(500);
  resetAFKTimer();

  if (!network::metadata::VNServerInformation::featureSupported("sequence")) return;
  connect(&m_checkTimer, &QTimer::timeout, this, &Courtroom::checkAFKStatus);
  m_checkTimer.start(1000);
}

Courtroom::~Courtroom()
{
  courtroom::cleanup();
  courtroom::tabs::cleanupToggles();
  ThemeManager::get().ResetWidgetLists();
  cleanup_preload_readers();
  system::ConfigManager::setDefaultGamemode("");
  system::ConfigManager::setdefaultTimeOfDay("");
  audio::StopAll();
}

void Courtroom::set_area_list(QStringList p_area_list)
{
  m_area_list = p_area_list;
  list_areas();
}

void Courtroom::set_music_list(QStringList p_music_list)
{
  m_music_list = p_music_list;
  ui_bgm_filter->ReloadCategories();
  list_music();
}

void Courtroom::setup_courtroom()
{
  TimeDebugger::get().StartTimer("Courtroom Setup");
  courtroom::cleanup();
  load_shouts();
  load_free_blocks();
  load_sfx_list_theme();

  ui_flip->show();

  load_effects();
  if(ThemeManager::get().getReloadPending())
  {
    load_wtce();
    map_viewers();
    assign_readers_for_all_viewers();
    list_music();
  }
  list_areas();

  // Update widgets first, then check if everything is valid
  // This will also handle showing the correct shouts, effects and wtce buttons,
  // and cycling through them if the buttons that are supposed to be displayed
  // do not exist It will also take care of free blocks
  const bool l_chr_select_visible = ui_char_select_background->isVisible();

  set_widget_names();
  set_widgets();
  ThemeManager::get().deleteTabPanels();
  ThemeManager::get().createTabParent();
  set_widget_layers();

  reset_widget_toggles();

  // char select
  reconstruct_char_select();
  ui_char_select_background->setVisible(l_chr_select_visible);

  for (AOTimer *i_timer : qAsConst(ui_timers))
  {
    i_timer->redraw();
  }

  construct_playerlist_layout();
  TimeDebugger::get().EndTimer("Courtroom Setup");
  engine::system::theme::reloadMetadata();
  LuaBridge::LuaEventCall("OnCourtroomSetup");


  w_ViewportOverlay->raise();
}

void Courtroom::map_viewers()
{
  m_mapped_viewer_list.clear();

  // general ui elements
  m_mapped_viewer_list[SpriteGUI].append({
      ui_vp_chat_arrow->get_player(),
      ui_vp_chatbox->get_player(),
      ui_vp_clock->get_player(),
  });

  // backgrounds
  m_mapped_viewer_list[SpriteWeather].append({
    ui_vp_weather->get_player()
  });

  m_mapped_viewer_list[SpriteStage].append({
      ui_vp_background->get_player(),
      ui_vp_desk->get_player(),
  });

  // characters
  m_mapped_viewer_list[SpriteCharacter].append(ui_vp_player_char->get_player());

  m_mapped_viewer_list[SpritePairCharacter].append(ui_vp_player_pair->get_player());

  // shouts
  m_mapped_viewer_list[SpriteShout].append(ui_vp_objection->get_player());

  // effects
  m_mapped_viewer_list[SpriteEffect].append(ui_vp_effect->get_player());

  // stickers
  for (DRStickerViewer *i_sticker : qAsConst(ui_free_blocks))
  {
    m_mapped_viewer_list[SpriteSticker].append(i_sticker->get_player());
  }
}

void Courtroom::map_viewport_viewers()
{
  m_viewport_viewer_map.clear();
  m_viewport_viewer_map.insert(ViewportStageBack, ui_vp_background->get_player());
  m_viewport_viewer_map.insert(ViewportStageFront, ui_vp_desk->get_player());
  m_viewport_viewer_map.insert(ViewportCharacterPre, ui_vp_player_char->get_player());
  m_viewport_viewer_map.insert(ViewportCharacterIdle, ui_vp_player_char->get_player());
  m_viewport_viewer_map.insert(ViewportCharacterTalk, ui_vp_player_char->get_player());
  m_viewport_viewer_map.insert(ViewportPairCharacterIdle, ui_vp_player_pair->get_player());
  m_viewport_viewer_map.insert(ViewportEffect, ui_vp_effect->get_player());
  m_viewport_viewer_map.insert(ViewportShout, ui_vp_objection->get_player());
}

void Courtroom::map_viewport_readers()
{
  const QList<ViewportSprite> l_type_list = m_viewport_viewer_map.keys();
  for (const ViewportSprite i_type : qAsConst(l_type_list))
  {
    m_reader_cache.insert(i_type, m_viewport_viewer_map[i_type]->get_reader());
  }
}

mk2::SpriteReader::ptr Courtroom::get_viewport_reader(ViewportSprite p_type) const
{
  Q_ASSERT(m_viewport_viewer_map.contains(p_type));
  auto l_viewer = m_viewport_viewer_map[p_type];
  return l_viewer->get_reader();
}

void Courtroom::assign_readers_for_all_viewers()
{
  for (auto it = m_mapped_viewer_list.cbegin(); it != m_mapped_viewer_list.cend(); ++it)
  {
    const SpriteCategory l_category = it.key();
    const bool l_caching = ao_config->sprite_caching_enabled(l_category);
    assign_readers_for_viewers(l_category, l_caching);
  }
}

void Courtroom::cleanup_preload_readers()
{
  m_preloader_sync->clear();
  m_preloader_cache.clear();
}

void Courtroom::swap_viewport_reader(DRMovie *p_viewer, ViewportSprite p_type)
{
  Q_ASSERT(m_reader_cache.contains(p_type));
  mk2::SpriteReader::ptr l_reader = m_reader_cache.value(p_type);
  if (l_reader == p_viewer->get_reader())
  {
    return;
  }
  p_viewer->set_reader(l_reader);
}

void Courtroom::assign_readers_for_viewers(int p_category, bool p_caching)
{
  const SpriteCategory l_category = SpriteCategory(p_category);

  // viewport readers
  const auto &l_viewer_list = m_mapped_viewer_list[l_category];
  for (auto i_viewer : qAsConst(l_viewer_list))
  {
    mk2::SpriteReader::ptr l_new_reader;
    if (p_caching)
    {
      l_new_reader = mk2::SpriteReader::ptr(new mk2::SpriteDynamicReader);
    }
    else
    {
      l_new_reader = mk2::SpriteReader::ptr(new mk2::SpriteSeekingReader);
    }
    l_new_reader->set_file_name(i_viewer->get_file_name());

    const mk2::SpriteReader::ptr l_prev_reader = i_viewer->get_reader();
    const bool l_is_running = i_viewer->is_running();
    const bool l_is_visible = i_viewer->property("visible").toBool();

    { // update reader cache
      const QList<ViewportSprite> l_type_list = m_reader_cache.keys();
      for (const ViewportSprite i_type : qAsConst(l_type_list))
      {
        const mk2::SpriteReader::ptr l_current_reader = m_reader_cache.value(i_type);
        if (l_prev_reader == l_current_reader)
        {
          m_reader_cache.insert(i_type, l_new_reader);
        }
      }
    }

    i_viewer->set_reader(l_new_reader);
    if (l_is_running)
    {
      i_viewer->start();
    }
    i_viewer->setProperty("visible", l_is_visible);
  }
}

void Courtroom::enter_courtroom(int p_cid)
{
  qDebug() << "enter_courtroom";

  // unmute audio
  audio::Suppress(false);

  const QString l_prev_chr_name = get_character_ini();

  // character =================================================================
  if (user::ChangeCharacterId(p_cid)) update_default_iniswap_item();

  QLineEdit *l_current_field = ui_ooc_chat_message->hasFocus() ? ui_ooc_chat_message : nullptr;
  const int l_current_cursor_pos = ui_ooc_chat_message->cursorPosition();

  const QString l_chr_name = get_character_ini();

  rolechat::actor::IActorData *actor = engine::actor::user::switchCharacter(l_chr_name);
  ui_emotes->actorChange(actor);
  if(!actor->scalingPresets().empty())
  {
    rolechat::actor::ActorScalingPreset preset = actor->scalingPresets().at(0);
    ui_slider_scale->setValue(preset.scale);
    ui_slider_vertical_axis->setValue(preset.verticalAlign);
  }
  else
  {
    ui_slider_scale->setValue(1000);
    ui_slider_vertical_axis->setValue(0);
  }

  bool spectating = user::IsSpectator();

  if (spectating)
  {
    ao_config->clear_showname_placeholder();
    WorkshopDiscord::getInstance().setRichPresenceStateText("Spectating");
  }
  else
  {
    const QString l_showname = QString::fromStdString(actor->showname());
    const QString l_final_showname = l_showname.trimmed().isEmpty() ? l_chr_name : l_showname;
    ao_config->set_showname_placeholder(l_final_showname);

    WorkshopDiscord::getInstance().setRichPresenceStateText("Playing as " + l_final_showname.toStdString());

    QStringList l_content{l_chr_name, l_final_showname};
    if(network::metadata::VNServerInformation::featureSupported("outfits")) l_content.append(QString::fromStdString(engine::actor::user::retrieve()->outfit()));

    ao_app->send_server_packet(DRPacket("chrini", l_content));
  }
  const bool l_changed_chr = l_chr_name != l_prev_chr_name;
  if (l_changed_chr)
    set_character_position(QString::fromStdString(actor->side()));
  select_base_character_iniswap();
  refresh_character_content_url();

  ui_emotes->refreshSelection(l_changed_chr);
  ui_emotes->refreshEmotes(false);

  onAnimListItemChanged(nullptr, nullptr);
  load_current_character_sfx_list();
  ui_sfx_list->selectDefault();

  ui_emotes->setHidden(spectating);
  ui_emote_dropdown->setDisabled(spectating);
  ui_iniswap_dropdown->setDisabled(spectating);
  ui_ic_chat_message_field->setDisabled(spectating);
  set_character_position(QString::fromStdString(actor->side()));

  if(l_current_field != nullptr)
  {
    l_current_field->setFocus();
    l_current_field->setCursorPosition(l_current_cursor_pos);
  }
  else
  {
    ui_ic_chat_message->setFocus();
  }

  const QString l_showname = ao_config->showname();
  if (!l_showname.isEmpty() && !is_first_showname_sent)
    send_showname_packet(l_showname);

  ui_char_select_background->hide();
  on_pos_dropdown_changed();
}

void Courtroom::done_received()
{
  user::ChangeCharacterId(SpectatorId);

  audio::Suppress(true);

  set_char_select();
  set_char_select_page();

  show();

  ui_spectator->show();
}

void Courtroom::set_window_title(QString p_title)
{
  this->setWindowTitle(p_title);
}

void Courtroom::set_ambient(QString p_ambient_sfx)
{
  m_ambient_sfx = p_ambient_sfx.trimmed();
  play_ambient();
}

void Courtroom::play_ambient()
{
  QString l_ambient = m_ambient_sfx;
  if (l_ambient.isEmpty())
  {
    DRPosition l_position = m_position_map.get_position(m_chatmessage[CMPosition]);
    l_ambient = l_position.get_ambient_sfx();
  }

  QString l_filepath = ao_app->get_ambient_sfx_path(l_ambient);
  audio::effect::PlayAmbient(l_filepath.toStdString());
}

QString Courtroom::get_current_background() const
{
  QString l_tod = ConfigManager::timeOfDay();
  return m_background.background_tod_map.value(l_tod, m_background.background);
}

void Courtroom::updateWeather(QString weatherName, const QString &environment)
{
  const QString weatherDirectory = FS::Paths::FindDirectory("animations/weather/" + weatherName + "/");

  replays::recording::weatherChange(weatherName, environment);
  if(!FS::Checks::DirectoryExists(weatherDirectory) || weatherName.trimmed().isEmpty())
  {
    ui_vp_weather->set_file_name("");
    ui_vp_weather->stop();
    ui_vp_weather->hide();
    ui_vp_weather->setKeyframeAnimation("", "");
    audio::effect::PlayWeather("");
    LuaBridge::LuaEventCall("OnWeatherEnd");
    return;
  }

  const QString weatherAnimPath = weatherDirectory + "overlay.webp";
  auto l_viewer = ui_vp_weather->get_reader();
  const QString l_current_file_name = l_viewer->get_file_name();

  JSONReader weatherParam = JSONReader();
  weatherParam.ReadFromFile(weatherDirectory + "param.json");

  bool hideAnimation = false;
  QString soundName = weatherParam.getStringValue("sound");
  QString sequenceName = weatherParam.getStringValue("sequence");


  bool weatherBehind = weatherParam.getBoolValue("layered_behind");

  if(weatherBehind)
  {
    ui_vp_weather->setZValue(ViewportLayers_WeatherBehind);
  }
  else
  {
    ui_vp_weather->setZValue(ViewportLayers_Weather);
  }

  if(weatherParam.isValueExists(environment))
  {
    weatherParam.SetTargetObject(environment);
    if(weatherParam.isValueExists("sound"))
    {
      soundName = weatherParam.getStringValue("sound");
    }
    hideAnimation = weatherParam.getBoolValue("hide");
    if(weatherParam.isValueExists("sequence"))
    {
      sequenceName = weatherParam.getStringValue("sequence");
    }
  }

  audio::effect::PlayWeather(ao_app->get_ambient_sfx_path(soundName).toStdString());

  if(!hideAnimation)
  {
    if (weatherAnimPath != l_current_file_name)
    {
      LuaBridge::LuaEventCall("OnWeatherChange", weatherName.toStdString());
      ui_vp_weather->set_reader(mk2::SpriteReader::ptr(new mk2::SpriteCachingReader));
      ui_vp_weather->set_file_name(weatherAnimPath);
      ui_vp_weather->setKeyframeAnimation("weather/" + weatherName, sequenceName);
    }
    ui_vp_weather->start();
    ui_vp_weather->show();
  }
  else
  {
    ui_vp_weather->set_file_name("");
    ui_vp_weather->stop();
    ui_vp_weather->hide();
  }
}

void Courtroom::playEffect(const QString &effectName, const QString &charaName)
{
  if(ui_vp_effect->is_running()) return;
  if (!effectName.isEmpty() && effectName != "effect_shake") // check to prevent crashing
  {
    bool once = true;

    audio::effect::Play(ao_app->get_sfx(effectName).toStdString());
    ui_vp_effect->stop();
    ui_vp_effect->set_file_name(ao_app->get_effect_anim_path(effectName));

    ui_vp_effect->set_play_once(once);
    ui_vp_effect->start();
  }
}

void Courtroom::update_background_scene()
{
  const QString l_prev_background_name = m_background_name;
  m_background_name = get_current_background();

  SceneManager::get().execLoadPlayerBackground(m_background_name);

  if (l_prev_background_name.isEmpty() || l_prev_background_name != m_background_name)
  {
    const QString l_positions_ini = ao_app->find_asset_path(ao_app->get_background_path(m_background_name) + "/" + "positions.ini");
    if (l_positions_ini.isEmpty() || !m_position_map.load_file(l_positions_ini))
    {
      m_position_map = get_legacy_background(m_background_name);
    }
  }

  const QString l_position_id = m_chatmessage[CMPosition];
  DRPosition l_position = m_position_map.get_position(m_chatmessage[CMPosition]);

  {
    const QString l_file_name = SceneManager::get().getBackgroundPath(l_position_id);
    ui_vp_background->set_file_name(l_file_name);
  }

  {
    const QString l_file_name = SceneManager::get().getForegroundPath(l_position_id);
    ui_vp_desk->set_file_name(l_file_name);
  }

  if (m_preloader_sync->is_waiting())
  {
    preload_chatmessage(m_pre_chatmessage);
  }

  display_background_scene();
}

void Courtroom::display_background_scene()
{
  ui_vp_background->show();
  ui_vp_background->set_play_once(false);
  swap_viewport_reader(ui_vp_background, ViewportStageBack);
  ui_vp_background->start();
  if (!ui_vp_background->is_valid())
  {
    ui_vp_background->hide();
  }

  ui_vp_desk->show();
  ui_vp_desk->set_play_once(false);
  swap_viewport_reader(ui_vp_desk, ViewportStageFront);
  ui_vp_desk->start();
  if (!ui_vp_desk->is_valid() || m_chatmessage[CMDeskModifier] == "0")
  {
    ui_vp_desk->hide();
  }

  play_ambient();
}

DRPositionMap Courtroom::get_legacy_background(QString p_background)
{
  DRPositionMap l_position_map;

  const auto &l_legacy_map = DRPositionMap::LEGACY_POSITION_MAP;
  for (auto it = l_legacy_map.cbegin(); it != l_legacy_map.cend(); ++it)
  {
    QString l_position_id = it.key();
    DRPosition l_position = it.value();

    auto l_get_position_filename = [this, &p_background](QString p_filename) {
      const QFileInfo l_filepath = ao_app->get_background_sprite_noext_path(p_background, p_filename);
      return l_filepath.fileName();
    };
    l_position.set_back(l_get_position_filename(l_position.get_back()));
    l_position.set_front(l_get_position_filename(l_position.get_front()));
    l_position_map.set_position(l_position_id, l_position);
  }

  return l_position_map;
}

DRAreaBackground Courtroom::get_background()
{
  return m_background;
}

void Courtroom::set_background(DRAreaBackground p_background)
{
  replays::recording::backgroundChange(p_background.background);
  m_background = p_background;

  QStringList l_background_list{m_background.background};
  for (auto it = m_background.background_tod_map.cbegin(); it != m_background.background_tod_map.cend(); ++it)
  {
    l_background_list.append(it.value());
  }

  QStringList l_missing_backgrounds;
  for (const QString &i_background : qAsConst(l_background_list))
  {
    const QString l_background_path = ao_app->get_case_sensitive_path(ao_app->get_background_path(i_background));
    if (!FS::Checks::DirectoryExists(l_background_path))
    {
      l_missing_backgrounds.append(i_background);
    }
  }

  if (!l_missing_backgrounds.isEmpty())
  {
    ui_ooc_chatlog->append_chatmessage("[WARNING]", "Missing background(s) detected: " + l_missing_backgrounds.join(", "));
  }

  update_background_scene();
}

void Courtroom::set_tick_rate(const int p_tick_rate)
{
  if (p_tick_rate < 0)
  {
    m_server_tick_rate.reset();
    return;
  }
  m_server_tick_rate = p_tick_rate;
}

void Courtroom::set_music_text(QString p_text)
{
  ui_vp_music_name->setText(p_text);
  update_music_text_anim();
}

void Courtroom::update_music_text_anim()
{
  RPRect res_a = engine::system::theme::getDimensions("music_name", ThemeSceneType::SceneType_Courtroom);
  RPRect res_b = engine::system::theme::getDimensions("music_area", ThemeSceneType::SceneType_Courtroom);

  float speed = static_cast<float>(ao_app->current_theme->get_music_name_speed());
  QFontMetrics fm(ui_vp_music_name->font());
  int dist;
  if (ao_app->current_theme->read_config_bool("enable_const_music_speed"))
    dist = res_b.width;
  else
    dist = fm.horizontalAdvance(ui_vp_music_name->toPlainText());
  int time = static_cast<int>(1000000 * dist / speed);
  music_anim->setLoopCount(-1);
  music_anim->setDuration(time);
  music_anim->setStartValue(QRect(res_b.width + res_b.x, res_a.y, res_a.width, res_a.height));
  music_anim->setEndValue(QRect(-dist + res_a.x, res_a.y, res_a.width, res_a.height));
  music_anim->start();
}

void Courtroom::handle_clock(QString time)
{
  m_current_clock = time.toInt();
  if (m_current_clock < 0)
    m_current_clock = -1;
  qInfo() << QString("Clock time changed to %1").arg(m_current_clock);

  ui_vp_clock->hide();

  replays::recording::hourChange(time);

  if (m_current_clock == -1)
  {
    qInfo() << "Unknown time; no asset to be used.";
    return;
  }

  qDebug() << "Displaying clock asset...";
  QString clock_filename = "hours/" + QString::number(m_current_clock);
  const QString asset_path = ao_app->find_theme_asset_path(clock_filename, FS::Formats::SupportedImages());
  if (asset_path.isEmpty())
  {
    qDebug() << "Asset not found; aborting.";
    return;
  }
  ui_vp_clock->set_theme_image(clock_filename);
  ui_vp_clock->start();
  ui_vp_clock->show();
}

bool Courtroom::is_area_music_list_separated()
{
  return ao_app->current_theme->read_config_bool("enable_music_and_area_list_separation");
}

void Courtroom::list_music()
{
  const QBrush l_song_brush = ao_app->current_theme->get_widget_settings_color("music_list", "courtroom", "found_song", "found_song_color");
  const QBrush l_missing_song_brush = ao_app->current_theme->get_widget_settings_color("music_list", "courtroom", "missing_song", "missing_song_color");

  ui_music_list->clear();
  for (const QString &i_song : qAsConst(m_music_list))
  {
    DRAudiotrackMetadata l_track(i_song);
    QListWidgetItem *l_item = new QListWidgetItem(l_track.title(), ui_music_list);
    l_item->setData(Qt::UserRole, l_track.filename());
    if (l_track.title() != l_track.filename())
      l_item->setToolTip(l_track.filename());
    const QString l_song_path = ao_app->find_asset_path({ao_app->get_music_path(i_song)}, FS::Formats::SupportedAudio());
    l_item->setBackground(l_song_path.isEmpty() ? l_missing_song_brush : l_song_brush);
  }

  ui_music_list->filterList(ui_music_search->text());
}

void Courtroom::list_areas()
{

  const QBrush l_area_brush = ao_app->current_theme->get_widget_settings_color("area_list", "courtroom", "area_free", "area_free_color");
  ui_area_list->clear();
  for (const QString &i_item_name : qAsConst(m_area_list))
  {
    QListWidgetItem *l_item = new QListWidgetItem(i_item_name, ui_area_list);
    l_item->setBackground(l_area_brush);
  }
  ui_area_list->filterList(ui_area_search->text());
}

void Courtroom::list_note_files()
{
  QString f_config = FS::Paths::BasePath() + CONFIG_FILESABSTRACT_INI;
  QFile f_file(f_config);
  if (!f_file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Couldn't open" << f_config;
    return;
  }

  QString f_filestring = "";
  QString f_filename = "";

  QTextStream in(&f_file);

  QVBoxLayout *f_layout = ui_note_area->m_layout;

  while (!in.atEnd())
  {
    QString line = in.readLine().trimmed();

    QStringList f_contents = line.split("=");
    if (f_contents.size() < 2)
      continue;

    int f_index = f_contents.at(0).toInt();
    f_filestring = f_filename = f_contents.at(1).trimmed();

    if (f_contents.size() > 2)
      f_filename = f_contents.at(2).trimmed();

    while (f_index >= f_layout->count())
      on_add_button_clicked();

    AONotePicker *f_notepicker = static_cast<AONotePicker *>(f_layout->itemAt(f_index)->widget());
    f_notepicker->ui_line->setText(f_filename);
    f_notepicker->m_file = f_filestring;
  }
}

QString Courtroom::get_current_position()
{
  if (ui_pos_dropdown->currentIndex() == DefaultPositionIndex)
  {
    return QString::fromStdString(engine::actor::user::retrieve()->side());
  }
  return ui_pos_dropdown->currentData(Qt::UserRole).toString();
}

void Courtroom::load_note()
{
  // Do not attempt to load anything if no file was chosen. This makes it so
  // that notepad text is kept in client if the user has decided not to choose a
  // file to save to. Of course, this is ephimeral storage, it will be erased
  // after the client closes or when the user decides to load a file.
  if (current_file.isEmpty())
    return;
  QString f_text = ao_app->read_note(current_file);
  ui_vp_notepad->setText(f_text);
}

void Courtroom::save_note()
{
  QString f_text = ui_vp_notepad->toPlainText();

  ao_app->write_note(f_text, current_file);
}

void Courtroom::save_textlog(QString p_text)
{
  QString f_file = FS::Paths::BasePath() + icchatlogsfilename;

  ao_app->append_note("[" + QTime::currentTime().toString() + "]" + p_text, f_file);
}

void Courtroom::append_server_chatmessage(QString p_name, QString p_message)
{
  if(!LuaBridge::LuaEventCall("OOCMessageEvent", p_name.toStdString(), p_message.toStdString()))
  {
    ui_ooc_chatlog->append_chatmessage(p_name, p_message);
    if (ao_config->log_is_recording_enabled())
      save_textlog("(OOC)" + p_name + ": " + p_message);

    replays::recording::messageSystem(p_name, p_message);
    LuaBridge::LuaEventCall("OnOOCMessage", p_name.toStdString(), p_message.toStdString());
  }
}

void Courtroom::ignore_next_showname()
{
  is_next_showname_ignored = true;
}

/**
 * @brief Send a packet to set the showname of the user to
 * the server.
 * @param p_showname The showname.
 */
void Courtroom::send_showname_packet(QString p_showname)
{
  if (is_next_showname_ignored)
  {
    is_next_showname_ignored = false;
    return;
  }

  is_first_showname_sent = true;

  ao_app->send_server_packet(DRPacket("SN", {p_showname}));
}

void Courtroom::on_showname_changed(QString p_showname)
{
  ui_ic_chat_showname->setText(p_showname);
  send_showname_packet(p_showname);
}

void Courtroom::on_pair_offset_changed()
{
  ao_app->send_server_packet(DRPacket("POFF", {QString::number(ui_slider_horizontal_axis->value())}));
}

void Courtroom::OnPlayerOffsetsChanged(int value)
{
  if(!network::metadata::VNServerInformation::featureSupported("outfits")) return;
  if(message::recentMessage().modifiers.Hidden) return;
  bool intParse = false;
  int speakerClientId = m_chatmessage[CMClientId].toInt(&intParse);

  DRCharacterMovie* targetCharacter = nullptr;
  rolechat::actor::ActorScalingMode targetScaling = rolechat::actor::ActorScalingMode::AutomaticScaling;

  if(user::getClientId() == speakerClientId)
  {
    if(message::recentMessage().characterFolder != engine::actor::user::name()) return;
    targetCharacter = ui_vp_player_char;
    if(m_SpeakerActor != nullptr)
      targetScaling = m_SpeakerActor->scalingMode();
  }
  if(speakerClientId == user::partner::clientId())
  {
    if(message::pair::getCharacter() != engine::actor::user::name()) return;
    targetCharacter = ui_vp_player_pair;
    if(m_PairActor != nullptr)
      targetScaling = m_PairActor->scalingMode();
  }

  double playerScale = (double)ui_slider_scale->value() / 1000.0f;
  if(targetCharacter == nullptr) return;

  targetCharacter->setVerticalOffset(ui_slider_vertical_axis->value());

  static double lastScale = 0.0f;

  targetCharacter->setHorizontalOffset(ui_slider_horizontal_axis->value());

  if(lastScale != playerScale)
  {
    if (targetCharacter->is_running())
      targetCharacter->stop();
    targetCharacter->start(targetScaling, playerScale);
    lastScale = playerScale;
  }

}

void Courtroom::on_showname_placeholder_changed(QString p_showname_placeholder)
{
  const QString l_showname(p_showname_placeholder.trimmed().isEmpty() ? localization::getText("TEXTBOX_SHOWNAME") : p_showname_placeholder);
  ui_ic_chat_showname->setPlaceholderText(l_showname);
  ui_ic_chat_showname->setToolTip(l_showname);
}

void Courtroom::on_character_ini_changed()
{
  enter_courtroom(user::GetCharacterId());
}

void Courtroom::on_ic_message_return_pressed()
{
  if (ui_ic_chat_message_field->text().isEmpty())
    return;

  if ((anim_state < 3 || text_state < 2) && m_shout_state == 0)
    return;

  // MS
  // deskmod
  // pre-emote
  // character
  // emote
  // message
  // side
  // sfx-name
  // emote_modifier
  // char_id
  // sfx_delay
  // objection_modifier
  // evidence
  // placeholder
  // realization
  // text_color
  // video_name
  // show_emote

  if(m_current_chat_type == ChatTypes::Think)
  {
      send_ooc_packet({"/think " + ui_ic_chat_message_field->text()});
      ui_ic_chat_message_field->clear();
      ui_ic_chat_message_field->clearTags();
      return;
  }

  if(m_current_chat_type == ChatTypes::Shout)
  {
      send_ooc_packet({"/shout " + ui_ic_chat_message_field->text()});
      ui_ic_chat_message_field->clear();
      ui_ic_chat_message_field->clearTags();
      return;
  }

  LuaBridge::LuaEventCall("OnICMessageSend");

  QStringList packet_contents;

  const ActorEmote &l_emote = ui_emotes->getSelectedEmote();

  const QString l_desk_modifier = l_emote.desk_modifier == -1 ? QString("chat") : QString::number(l_emote.desk_modifier);
  packet_contents.append(l_desk_modifier);

  packet_contents.append(QString::fromStdString(l_emote.anim));

  packet_contents.append(get_character_ini());

  if (ui_hide_character->isChecked())
    packet_contents.append("../../misc/blank");
  else
    packet_contents.append(QString::fromStdString(l_emote.dialog));

  packet_contents.append(ui_ic_chat_message_field->text());

  packet_contents.append(get_current_position());

  // sfx file
  const QString l_sound_file = current_sfx_file();
  packet_contents.append(l_sound_file.isEmpty() ? "0" : l_sound_file);

  int l_emote_mod = l_emote.modifier;

  if (ui_pre->isChecked())
  {
    if (l_emote_mod == ZoomEmoteMod)
      l_emote_mod = PreZoomEmoteMod;
    else
      l_emote_mod = PreEmoteMod;
  }
  else
  {
    if (l_emote_mod == PreZoomEmoteMod)
      l_emote_mod = ZoomEmoteMod;
    else
      l_emote_mod = IdleEmoteMod;
  }

  if (m_shout_state != 0)
  {
    if (l_emote_mod == ZoomEmoteMod)
      l_emote_mod = PreZoomEmoteMod;
    else
      l_emote_mod = PreEmoteMod;
  }

  packet_contents.append(QString::number(l_emote_mod));
  packet_contents.append(QString::number(user::GetCharacterId()));

  if (l_emote.sound_file == current_sfx_file().toStdString())
    packet_contents.append(QString::number(l_emote.sound_delay));
  else
    packet_contents.append("0");

  packet_contents.append(QString::number(m_shout_state));

  // evidence
  packet_contents.append("0");

  QString f_flip = ui_flip->isChecked() ? "1" : "0";
  packet_contents.append(f_flip);

  packet_contents.append(QString::number(m_effect_state));

  QString f_text_color;
  if (m_text_color < 0)
    f_text_color = "0";
  else if (m_text_color > ui_text_color->count())
    f_text_color = "0";
  else
    f_text_color = QString::number(m_text_color);
  packet_contents.append(f_text_color);

  // showname
  packet_contents.append(ao_config->showname());

  // video name
  packet_contents.append(!QString::fromStdString(l_emote.video_file).isEmpty() ? QString::fromStdString(l_emote.video_file) : "0");

  // hide character
  packet_contents.append(QString::number(ui_hide_character->isChecked()));

  if(network::metadata::VNServerInformation::featureSupported("outfits"))
  {
    if(l_emote.ignore_offsets)
    {
      packet_contents.append(QString::number(500));
      packet_contents.append(QString::number(0));
      packet_contents.append(QString::number(1000));
    }
    else
    {
      packet_contents.append(QString::number(ui_slider_horizontal_axis->value()));
      packet_contents.append(QString::number(ui_slider_vertical_axis->value()));
      packet_contents.append(QString::number(ui_slider_scale->value()));
    }
  }


  if(network::metadata::VNServerInformation::featureSupported("sequence"))
  {
    packet_contents.append(QString::fromStdString(l_emote.outfitName));
    packet_contents.append(QString::fromStdString(courtroom::lists::getAnimation()));
    QStringList layers;
    int layerCount = 0;
    for(const ActorLayer &layer : l_emote.emoteOverlays)
    {
      if(engine::actor::user::layerState(layer.toggleName) && layerCount < 4)
      {
        layers.append(engine::system::encoding::text::EncodePacketContents({QString::fromStdString(layer.spriteName), QString::fromStdString(layer.spriteOrder), QString::number(layer.layerOffset.x), QString::number(layer.layerOffset.y), QString::number(layer.layerOffset.width), QString::number(layer.layerOffset.height), QString::fromStdString(layer.offsetName)}));
        layerCount += 1;
      }
    }
    packet_contents.append(engine::system::encoding::text::EncodeBase64(layers));
  }


  if(network::metadata::VNServerInformation::featureSupported("tags"))
  {
    QStringList tags;
    for(MessageTag tag : ui_ic_chat_message_field->getTags())
    {
      tags.append(engine::system::encoding::text::EncodePacketContents({QString::number(tag.index), tag.value}));
    }
    packet_contents.append(engine::system::encoding::text::EncodeBase64(tags));
  }

  ao_app->send_server_packet(DRPacket("MS", packet_contents));
}

void Courtroom::handle_ic_message_length()
{
  LuaBridge::LuaEventCall("OnICTextChanged");
  const int l_length = ui_ic_chat_message_field->text().length();
  const int l_max_length = ui_ic_chat_message_field->maxLength();
  ui_ic_chat_message_counter->setText(QString::number(l_max_length - l_length));
  ui_ic_chat_message_counter->setVisible(l_length >= int(l_max_length * (ao_config->message_length_threshold() * 0.01)));
  if (l_length == l_max_length)
  {
    QToolTip::showText(QCursor::pos(), QString(tr("Your message cannot be longer than %1 character(s).")).arg(ui_ic_chat_message_field->maxLength()));
  }

  if(l_length == 0) return;
  if(!network::metadata::VNServerInformation::featureSupported("sequence")) return;

  const int currentUptime = RuntimeLoop::uptime();
  if(m_lastTypingPacket == 0 || (currentUptime - m_lastTypingPacket) > (1000 * 10))
  {
    ao_app->send_server_packet(DRPacket("STATUS", {QString::number(UserState_Typing), QString::number(true)}));
    m_lastTypingPacket = currentUptime;
  }

}

void Courtroom::handle_acknowledged_ms()
{
  ui_ic_chat_message_field->clear();
  ui_ic_chat_message_field->clearTags();

  // reset states
  ui_pre->setChecked(ao_config->always_pre_enabled());

  reset_shout_buttons();
  reset_effect_buttons();
  ui_sfx_list->clearSelection();
}

void Courtroom::next_chatmessage(QStringList p_chatmessage)
{
  if (p_chatmessage.length() < MINIMUM_MESSAGE_SIZE)
  {
    return;
  }

  while (p_chatmessage.length() < OPTIMAL_MESSAGE_SIZE)
  {
    p_chatmessage.append(QString{});
  }

  if(!m_chatmessage[CMMessage].trimmed().isEmpty())
  {
    while(m_tick_step <= m_chatmessage[CMMessage].length() - 1)
    {
      next_chat_letter();
    }
  }

  message::incomingMessage(p_chatmessage);

  const int l_message_chr_id = p_chatmessage[CMChrId].toInt();
  const bool l_system_speaking = l_message_chr_id == SpectatorId;

  m_SpeakerActor = engine::actor::repository::retrieve(p_chatmessage[CMChrName]);
  if(!p_chatmessage[CMOutfitName].isEmpty())
  {
    m_SpeakerActor->switchOutfit(p_chatmessage[CMOutfitName].toStdString());
  }

  if(message::pair::isActive())
  {
    m_PairActor = engine::actor::repository::retrieve(message::pair::getCharacter());

    m_PairScaling = rolechat::actor::ActorScalingMode::AutomaticScaling;

    if(m_PairActor != nullptr)
    {
      m_PairScaling = m_PairActor->scalingMode();
      m_PairScale = message::pair::scaleOffset();
    }

  }


  m_ActorScale = 1.0;

  int scaleValue = p_chatmessage[CMOffsetS].trimmed().toInt();
  if(!p_chatmessage[CMOffsetS].isEmpty())
  {
    m_ActorScale = (double)scaleValue / 1000.0f;
  }




  m_ActorScaling = rolechat::actor::ActorScalingMode::AutomaticScaling;

  if(m_SpeakerActor != nullptr)
  {
    m_ActorScaling = m_SpeakerActor->scalingMode();
  }



  QString l_showname = p_chatmessage[CMShowName];
  if (l_showname.isEmpty() && !l_system_speaking)
  {
    l_showname = QString::fromStdString(m_SpeakerActor->showname());
  }

  QString l_message = QString(p_chatmessage[CMMessage]).remove(QRegularExpression("(?<!\\\\)(\\{|\\})")).replace(QRegularExpression("\\\\(\\{|\\})"), "\\1");
  if(l_message.startsWith("<a>"))
  {
    l_message = l_message.mid(3);
  }
  if (l_message_chr_id == SpectatorId)
  {
    append_system_text(l_showname, l_message);
  }
  else if (l_message_chr_id >= 0 && l_message_chr_id < CharacterRepository::serverListLength())
  {
    const int l_client_id = p_chatmessage[CMClientId].toInt();
    append_ic_text(l_showname, l_message, false, false, l_client_id, user::GetCharacterId() == l_message_chr_id, true);


    QScrollBar *l_scrollbar = ui_ic_chatlog->verticalScrollBar();
    const int l_scroll_pos = l_scrollbar->value();
    bool l_scroll_limt = l_scroll_pos == l_scrollbar->maximum();




    QTextCursor cursor2 = ui_ic_chatlog->textCursor();
    cursor2.movePosition(QTextCursor::End);
    ui_ic_chatlog->setTextCursor(cursor2);
    m_iclog_cursor_position = cursor2.position();

    l_scrollbar->setValue(l_scroll_limt ? l_scrollbar->maximum() : l_scroll_pos);

    if (ao_config->log_is_recording_enabled() && !l_message.isEmpty())
    {
      save_textlog(l_showname + ": " + l_message);
    }
  }


  { // clear interface if required
    bool l_ok;
    const int l_client_id = p_chatmessage[CMClientId].toInt(&l_ok);
    if (l_ok && l_client_id == user::getClientId())
    {
      handle_acknowledged_ms();
    }
  }
  SceneManager::get().RenderTransition();
  preload_chatmessage(p_chatmessage);
}

void Courtroom::reset_viewport()
{
  QStringList l_chatmessage;

  while (l_chatmessage.length() < OPTIMAL_MESSAGE_SIZE)
  {
    l_chatmessage.append(QString{});
  }
  l_chatmessage[CMChrId] = QString::number(SpectatorId);
  l_chatmessage[CMHideCharacter] = "1";
  l_chatmessage[CMClientId] = QString::number(SpectatorId);
  l_chatmessage[CMPosition] = m_chatmessage.value(CMPosition, "wit");

  next_chatmessage(l_chatmessage);
}

void Courtroom::preload_chatmessage(QStringList p_contents)
{
  cleanup_preload_readers();
  m_loading_timer->stop();
  m_pre_chatmessage = p_contents;
  m_game_state = GameState::Preloading;

  QMap<ViewportSprite, QString> l_file_list;
  const QString l_position_id = m_pre_chatmessage[CMPosition];
  const QString l_character = m_pre_chatmessage[CMChrName];
  const QString l_emote_anim = m_pre_chatmessage[CMPreAnim];
  const QString l_emote = m_pre_chatmessage[CMEmote];
  const QString l_outfit = m_pre_chatmessage[CMOutfitName];
  const int l_effect_id = m_pre_chatmessage[CMEffectState].toInt();
  const int l_shout_id = m_pre_chatmessage[CMShoutModifier].toInt();

  { // backgrounds
    DRPosition l_position = m_position_map.get_position(l_position_id);
    l_file_list.insert(ViewportStageBack, SceneManager::get().getBackgroundPath(l_position_id));
    l_file_list.insert(ViewportStageFront, SceneManager::get().getForegroundPath(l_position_id));
  }

  // characters
  l_file_list.insert(ViewportCharacterPre, fs::characters::getSpritePathPre(l_character, l_emote_anim));
  l_file_list.insert(ViewportCharacterIdle, fs::characters::getSpritePathIdle(l_character, l_emote));
  l_file_list.insert(ViewportCharacterTalk, fs::characters::getSpritePathTalk(l_character, l_emote));

  l_file_list.insert(ViewportPairCharacterIdle, fs::characters::getSpritePathIdle(message::pair::getCharacter(), message::pair::getEmote()));

  // shouts
  l_file_list.insert(ViewportShout, ao_app->get_shout_sprite_path(l_character, get_shout_name(l_shout_id), l_outfit));

  // effects
  QString effect_name = get_effect_name(l_effect_id);
  if(effect_name == "effect_shake")
  {
    play_screenshake_anim();
  }
  else
  {
    l_file_list.insert(ViewportEffect, ao_app->get_effect_anim_path(effect_name));
  }



  for (auto it = l_file_list.cbegin(); it != l_file_list.cend(); ++it)
  {
    const ViewportSprite l_type = it.key();
    const QString &l_file_name = it.value();

    auto l_viewer = m_viewport_viewer_map.value(l_type);
    const QString l_current_file_name = l_viewer->get_file_name();

    // reuse readers when available
    mk2::SpriteReader::ptr l_reader = l_viewer->get_reader();
    if (l_file_name != l_current_file_name)
    {
      const SpriteCategory l_category = viewport_sprite_to_sprite_category(l_type);
      if (ao_config->sprite_caching_enabled(l_category))
      {
        l_reader = mk2::SpriteReader::ptr(new mk2::SpriteDynamicReader);
      }
      else
      {
        l_reader = mk2::SpriteReader::ptr(new mk2::SpriteSeekingReader);
      }
      l_reader->set_file_name(l_file_name);
    }
    m_preloader_cache.insert(l_type, l_reader);
    m_preloader_sync->add(l_reader);
  }
  m_loading_timer->start();
  m_preloader_sync->start();
}

void Courtroom::start_chatmessage()
{
  m_preloader_sync->clear();
  m_reader_cache = std::move(m_preloader_cache);

  m_loading_timer->stop();
  ui_vp_loading->hide();

  m_tick_timer->stop();
  m_chatmessage = m_pre_chatmessage;

  m_appendMessage = m_chatmessage[CMMessage].startsWith("<a>");

  if(m_appendMessage)
  {
    QString appendPrefix = ui_vp_message->toPlainText().right(1) == " " ? "" : " ";
    m_chatmessage[CMMessage] = appendPrefix + m_chatmessage[CMMessage].mid(3);
  }


  int incomingSender = m_chatmessage[CMClientId].toInt();

  if(m_messageSender != incomingSender)
    m_appendMessage = false;

  m_messageSender = incomingSender;

  m_game_state = GameState::Processing;

  handle_chatmessage();
}

void Courtroom::handle_chatmessage()
{
  qDebug() << "handle_chatmessage";
  LuaBridge::OnCharacterMessage(m_chatmessage[CMShowName].toStdString(), m_chatmessage[CMChrName].toStdString(), m_chatmessage[CMEmote].toStdString(), m_chatmessage[CMMessage].toStdString(), m_chatmessage[CMMessage].trimmed().isEmpty());
  m_hide_character = m_chatmessage[CMHideCharacter].toInt();
  m_play_pre = false;
  m_play_zoom = false;
  const int l_emote_mod = m_chatmessage[CMEmoteModifier].toInt();
  switch (l_emote_mod)
  {
  case IdleEmoteMod:
  default:
    break;
  case PreEmoteMod:
    m_play_pre = true;
    break;
  case ZoomEmoteMod:
    m_play_zoom = true;
    break;
  case PreZoomEmoteMod:
    m_play_pre = true;
    m_play_zoom = true;
    break;
  }

  m_speaker_chr_id = m_chatmessage[CMChrId].toInt();
  is_system_speaking = (m_speaker_chr_id == SpectatorId);

  if (m_speaker_chr_id != SpectatorId && (m_speaker_chr_id < 0 || m_speaker_chr_id >= CharacterRepository::serverListLength()))
  {
    post_chatmessage();
    return;
  }

  const QString l_message = QString(m_chatmessage[CMMessage]).remove(QRegularExpression("(?<!\\\\)(\\{|\\})")).replace(QRegularExpression("\\\\(\\{|\\})"), "\\1");
  chatmessage_is_empty = l_message.trimmed().isEmpty();
  m_msg_is_first_person = false;

  // reset our ui state if client just spoke
  if (user::IsSpectator() && is_system_speaking == false)
  {
    // update first person mode status
    m_msg_is_first_person = ao_app->get_first_person_enabled();
  }

  qDebug() << "handle_chatmessage";

  // We actually DO wanna fail here if the showname is empty but the system is speaking.
  // Having an empty showname for system is actually what we expect.


  QString f_showname = m_chatmessage[CMShowName];
  if (f_showname.isEmpty() && !is_system_speaking)
  {
    f_showname = QString::fromStdString(m_SpeakerActor->showname());
  }
  m_speaker_showname = f_showname;

  ui_vp_chat_arrow->hide();
  audio::effect::StopAll();

  text_state = 0;
  anim_state = 0;
  stop_chat_timer();
  ui_vp_objection->stop();

  m_message_color_name.clear();
  m_message_color_stack.clear();

  // reset effect
  ui_vp_effect->stop();
  ui_vp_effect->hide();

  if(!m_appendMessage)
    ui_vp_message->clear();

  ui_vp_chatbox->hide();
  ui_vp_showname->hide();
  ui_vp_showname_image->hide();
  ui_vp_showname->setText(m_speaker_showname);

  /**
   * WARNING No check prior to changing will cause an unrecoverable
   * exception. You have been warned!
   *
   * Qt Version: <= 5.15
   */
  if (!ui_video->isVisible())
  {
    ui_video->show();
  }
  ui_video->play_character_video(m_chatmessage[CMChrName], m_chatmessage[CMVideoName]);
}

QString Courtroom::get_shout_name(int p_shout_index)
{
  if (p_shout_index < 1 || p_shout_index > ui_shouts.length())
  {
    return QString{};
  }

  return shout_names.at(p_shout_index - 1);
}

QString Courtroom::get_effect_name(int effect_index)
{
  if (effect_index < 1 || effect_index > ui_effects.length())
  {
    return QString{};
  }

  return effect_names.at(effect_index - 1);
}

void Courtroom::video_finished()
{
  /**
   * WARNING No check prior to changing will cause an unrecoverable
   * exception. You have been warned!
   *
   * Qt Version: <= 5.15
   */
  if (ui_video->isVisible())
  {
    ui_video->hide();
  }

  const QString l_character = m_chatmessage[CMChrName];
  const int l_shout_index = m_chatmessage[CMShoutModifier].toInt();

  const QString l_shout_name = get_shout_name(l_shout_index);
  if (l_shout_name.isEmpty())
  {
    handle_chatmessage_2();
    return;
  }

  qDebug() << "[viewport] Starting shout..." << l_shout_name;
  m_play_pre = true;
  ui_vp_objection->set_play_once(true);
  swap_viewport_reader(ui_vp_objection, ViewportShout);
  ui_vp_objection->start();
  audio::shout::Play(l_character.toStdString(), l_shout_name.toStdString());
}

void Courtroom::objection_done()
{
  if (anim_state != 0)
  {
    return;
  }
  handle_chatmessage_2();
}

void Courtroom::handle_chatmessage_2() // handles IC
{

  currentDelayLeft = 3000;
  customMessageSpeed = 0;
  int selfOffset = message::horizontalOffset();
  int otherOffset = message::pair::horizontalOffset();

  QString offsetTextbox = "left";

  if(selfOffset > otherOffset) offsetTextbox = "right";

  if(message::pair::isLeader())
  {
    ui_vp_player_char->setZValue(ViewportLayers_PlayerBack);
    ui_vp_player_pair->setZValue(ViewportLayers_PlayerFront);
  }
  else
  {
    ui_vp_player_char->setZValue(ViewportLayers_PlayerFront);
    ui_vp_player_pair->setZValue(ViewportLayers_PlayerBack);
  }


  if(!m_chatmessage[CMOffsetH].isEmpty())
  {
    int hOffset = m_chatmessage[CMOffsetH].toInt();

    if(hOffset != 0)
      selfOffset = hOffset;
  }
  ui_vp_player_char->setHorizontalOffset(selfOffset);
  ui_vp_player_pair->setHorizontalOffset(otherOffset);
  int verticalValue = m_chatmessage[CMOffsetV].trimmed().toInt();

  if(m_chatmessage[CMOffsetV].isEmpty())
    ui_vp_player_char->setVerticalOffset(0);
  else
    ui_vp_player_char->setVerticalOffset(verticalValue);

  ui_vp_player_pair->setVerticalOffset(message::pair::verticalOffset());
  setup_screenshake_anim(0);
  qDebug() << "handle_chatmessage_2";
  ui_vp_player_char->stop();
  ui_vp_player_pair->stop();

  if(ao_app->current_theme->m_jsonLoaded)
  {
    if(!message::pair::isActive())
    {
      ui_vp_player_pair->hide();
      RPRect showname = ThemeManager::get().resizePosition(engine::system::theme::getPositionalDimensions("showname", "center"), ThemeManager::get().getViewporResize());
      RPRect l_MessagePos = ThemeManager::get().resizePosition(engine::system::theme::getPositionalDimensions("message", "center"), ThemeManager::get().getViewporResize());

      ui_vp_showname->move(showname.x, showname.y);
      ui_vp_showname->resize(showname.width, showname.height);
      ui_vp_message->move(l_MessagePos.x, l_MessagePos.y);
      ui_vp_message->resize(l_MessagePos.width, l_MessagePos.height);
      offsetTextbox = "center";
    }
    else
    {
      RPRect showname = ThemeManager::get().resizePosition(engine::system::theme::getPositionalDimensions("showname", offsetTextbox), ThemeManager::get().getViewporResize());
      RPRect l_MessagePos = ThemeManager::get().resizePosition(engine::system::theme::getPositionalDimensions("message", offsetTextbox), ThemeManager::get().getViewporResize());

      ui_vp_showname->move(showname.x, showname.y);
      ui_vp_showname->resize(showname.width, showname.height);
      ui_vp_message->move(l_MessagePos.x, l_MessagePos.y);
      ui_vp_message->resize(l_MessagePos.width, l_MessagePos.height);
    }


    setShownameFont(ui_vp_showname, "showname", offsetTextbox);
    setShownameFont(ui_vp_message, "message", offsetTextbox);
  }

  if (m_shout_reload_theme)
  {
    load_theme();
  }
  m_shout_reload_theme = false;

  const QString l_chatbox_name = ao_app->get_chat(m_chatmessage[CMChrName]);
  const bool l_is_self = (ao_config->log_display_self_highlight_enabled() && m_speaker_chr_id == user::GetCharacterId());

  if(!LuaBridge::LuaEventCall("ChatboxImageEvent", l_is_self, chatmessage_is_empty, offsetTextbox.toStdString()))
  {
    ui_vp_chatbox->set_chatbox_image(l_chatbox_name, l_is_self, chatmessage_is_empty, offsetTextbox);
  }

  if (!m_msg_is_first_person)
  {
    display_background_scene();
  }


  ui_vp_player_pair->setMirrored(message::pair::isFlipped());

  if (m_chatmessage[CMFlipState].toInt() == 1)
    ui_vp_player_char->setMirrored(true);
  else
    ui_vp_player_char->setMirrored(false);

  SceneManager::get().AnimateTransition();

  if (m_play_pre)
  {
    int sfx_delay = m_chatmessage[CMSoundDelay].toInt();
    m_sound_timer->start(sfx_delay);
    play_preanim();
    return;
  }
  handle_chatmessage_3();
}

void Courtroom::handle_chatmessage_3()
{
  QStringList tagInformation = engine::system::encoding::text::DecodeBase64(m_pre_chatmessage[CMMessageTags]);

  m_ProcessedTags.clear();

  for(QString tagInfo : tagInformation)
  {
    QStringList tagArray = system::encoding::text::DecodePacketContents(tagInfo);
    if(tagArray.count() == 2)
    {
      int tagPosition = tagArray[0].toInt();
      QVariantList tagArguments = engine::encoding::BinaryEncoder::decodeBase64(tagArray[1]);
      IncomingTagData data = {tagPosition, (MessageTagType)tagArguments.at(0).toInt(), tagArguments};
      m_ProcessedTags.append(data);
    }
  }

  qDebug() << "handle_chatmessage_3";

  ui_vp_player_char->set_play_once(false);

  if(message::pair::isActive())
  {
    ui_vp_player_pair->set_play_once(false);
  }


  setup_chat();

  int f_anim_state = 0;
  // BLUE is from an enum in datatypes.h
  bool text_is_blue = m_chatmessage[CMTextColor].toInt() == DR::CBlue;

  if (!text_is_blue && text_state == 1)
    // talking
    f_anim_state = 2;
  else
    // idle
    f_anim_state = 3;

  if (f_anim_state <= anim_state)
    return;

  ui_vp_player_char->stop();
  ui_vp_player_pair->stop();
  const QString f_char = m_chatmessage[CMChrName];
  const QString f_emote = m_chatmessage[CMEmote];

  if (!chatmessage_is_empty)
  {
    QString l_showname_image;
    if (ao_app->current_theme->read_config_bool("enable_showname_image"))
    {
      l_showname_image = ao_app->find_theme_asset_path("characters/" + f_char + "/showname_" + m_chatmessage[CMOutfitName], FS::Formats::StaticImages());

      if (l_showname_image.isEmpty())
        l_showname_image = ao_app->find_theme_asset_path("characters/" + f_char + "/showname", FS::Formats::StaticImages());

      if (l_showname_image.isEmpty())
        l_showname_image = ao_app->find_asset_path({fs::characters::getFilePath(f_char, "showname")}, FS::Formats::StaticImages());
    }

    if (!l_showname_image.isEmpty())
    {
      ui_vp_showname_image->set_image(l_showname_image);
      ui_vp_showname_image->show();
    }
    else
    {
      ui_vp_showname_image->hide();
      ui_vp_showname->show();
    }
  }

  //Setup Partner
  if(message::pair::isVisible())
  {
    ui_vp_player_pair->hide();
    if(ui_vp_player_pair->is_running())
    {
      ui_vp_player_pair->stop();
    }

    swap_viewport_reader(ui_vp_player_pair, ViewportPairCharacterIdle);
    ui_vp_player_pair->start(m_PairScaling, m_PairScale);
  }
  else
  {
    ui_vp_player_pair->hide();
  }

  // Path may be empty if
  // 1. Chat message was empty
  // 2. Enable showname images was false
  // 3. No valid showname image was found

  ui_vp_player_char->hide();
  ui_vp_player_char->clearImageLayers();
  ui_vp_player_pair->clearImageLayers();
  if (ui_vp_player_char->is_running())
  {
    ui_vp_player_char->stop();
  }
  switch (f_anim_state)
  {
  case 2:
    if (!m_hide_character && !m_msg_is_first_person)
    {
      swap_viewport_reader(ui_vp_player_char, ViewportCharacterTalk);
      ui_vp_player_char->start(m_ActorScaling, m_ActorScale);
      ui_vp_player_char->setLayerState(ViewportCharacterTalk);
    }
    anim_state = 2;
    break;
  default:
    qDebug() << "W: invalid anim_state: " << f_anim_state;
    [[fallthrough]];
  case 3:
    if (!m_hide_character && !m_msg_is_first_person)
    {
      swap_viewport_reader(ui_vp_player_char, ViewportCharacterIdle);
      ui_vp_player_char->start(m_ActorScaling, m_ActorScale);
      ui_vp_player_char->setLayerState(ViewportCharacterIdle);
    }
    anim_state = 3;
    break;
  }

  ui_vp_player_char->processOverlays(m_pre_chatmessage[CMSpriteLayers], m_pre_chatmessage[CMChrName], f_emote, m_pre_chatmessage[CMOutfitName]);
  ui_vp_player_pair->processOverlays(message::pair::getLayers(), message::pair::getCharacter(), message::pair::getEmote(), message::pair::getOutfit());

  {
    bool l_effect_index_result;
    const int l_effect_index = m_chatmessage[CMEffectState].toInt(&l_effect_index_result);
    if (l_effect_index_result)
    {
      const QString l_effect_name = get_effect_name(l_effect_index);
      if (!l_effect_name.isEmpty() && l_effect_name != "effect_shake") // check to prevent crashing
      {
        QStringList offset = ao_app->get_effect_offset(f_char, l_effect_index);
        ui_vp_effect->setPos(0, offset.at(1).toInt());
        int selfOffset = message::horizontalOffset();
        if(ao_app->current_theme->getEffectPairOffset(l_effect_index)) ui_vp_effect->setHorizontalOffset(selfOffset);

        QString s_eff = effect_names.at(l_effect_index - 1);
        QStringList f_eff = ao_app->get_effect(l_effect_index);

        bool once = f_eff.at(1).trimmed().toInt();

        QStringList overlay = ao_app->get_overlay(f_char, l_effect_index);
        QString overlay_name = overlay.at(0);
        QString overlay_sfx = overlay.at(1);

        if (overlay_sfx == "")
          overlay_sfx = ao_app->get_sfx(s_eff);
        audio::effect::Play(overlay_sfx.toStdString());

        if (overlay_name == "")
          overlay_name = s_eff;
        ui_vp_effect->set_play_once(once);
        swap_viewport_reader(ui_vp_effect, ViewportEffect);
        ui_vp_effect->start();
      }
    }
  }

  QString f_message = m_chatmessage[CMMessage];
  QStringList callwords = ao_app->get_callwords();

  for (const QString &word : callwords)
  {
    if (f_message.contains(word, Qt::CaseInsensitive))
    {
      audio::system::Play(ao_app->get_sfx("word_call").toUtf8());
      ao_app->alert(this);
      const QString name = "CLIENT";
      const QString message = ui_vp_showname->toPlainText() + " has called you via your callword \"" + word + "\": \"" + f_message + "\"";
      ui_ooc_chatlog->append_chatmessage(name, message);
      if (ao_config->log_is_recording_enabled())
        save_textlog("(OOC)" + name + ": " + message);
      break;
    }
  }

  calculate_chat_tick_interval();

  LuaBridge::LuaEventCall("OnMessageStart");
  start_chat_timer();
  message::pair::disable();

  const int clientId = m_chatmessage[CMClientId].toInt();
  const bool isLocalClient = clientId == user::getClientId();
  if(isLocalClient) m_lastTypingPacket = 0;
  else
  {
    setPlayerTyping(clientId, false);
  }

  bool animLoop = ui_vp_player_char->setCharacterAnimation(m_chatmessage[CMAnimSequence], m_chatmessage[CMChrName]);
  if(!animLoop && isLocalClient)
    ui_anim_list->selectDefault();

  ui_vp_player_pair->setCharacterAnimation(message::pair::getAnimation(), message::pair::getCharacter(), true);

}

void Courtroom::handelInvestigation(QString p_contents)
{
  if(w_ViewportOverlay == nullptr) return;
  w_ViewportOverlay->clearInteractions();
  QString l_PacketData = engine::system::encoding::text::DecodeBase64String(p_contents);
  JSONReader l_InvestigationJson = JSONReader();
  l_InvestigationJson.ReadFromString(l_PacketData);


  for (const QJsonValue &value : l_InvestigationJson.mDocument.array())
  {
    if (value.isObject())
    {
      l_InvestigationJson.SetTargetObject(value.toObject());
      QString l_ObjName = l_InvestigationJson.getStringValue("name");
      QString l_ObjDesc = l_InvestigationJson.getStringValue("desc");
      QRect l_ObjRect = l_InvestigationJson.getRectangleValue("rect");
      w_ViewportOverlay->addInteraction(l_ObjRect, l_ObjName, l_ObjDesc);
    }
  }
}

void Courtroom::handleScene(QStringList p_contents)
{
  while(p_contents.length() < 10)
  {
    p_contents.append("");
  }

  LuaBridge::LuaEventCall("OnSceneChange", p_contents.at(0).toStdString(), p_contents.at(2).toStdString());

  runtime::values::storeValue("area_name", p_contents.at(0).toStdString());
  runtime::values::storeValue("map_visual", p_contents.at(2).toStdString());

  ui_vp_wtce->setThemeAnimation("scene_transition");

  //DRAreaBackground l_area_bg;
  //l_area_bg.background = t_Contents.at(1);
  //l_CourtroomScene->set_background(l_area_bg);

  reset_viewport();
}

void Courtroom::on_chat_config_changed()
{
  update_ic_log(true);
}

void Courtroom::OnBgmFilterChanged()
{
  m_music_list = ui_bgm_filter->GetMusicList();
  list_music();
}

void Courtroom::onFlipTagActivated()
{
  ui_ic_chat_message_field->addTag(TagType_Flip, {});
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::onAnimationTag()
{
  auto item = ui_anim_list->currentItem();
  if (!item) return;

  ui_ic_chat_message_field->addTag(TagType_PlaySequence, { item->text() });
  ui_anim_list->setCurrentRow(-1);
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::CharacterSearchUpdated()
{
  m_current_chr_page = 0;
  set_char_select_page();
}

void Courtroom::load_ic_text_format()
{
  ui_ic_chatlog->ensurePolished();
  m_ic_log_format.base = QTextCharFormat();
  m_ic_log_format.base.setFont(ui_ic_chatlog->font());
  m_ic_log_format.base.setForeground(ui_ic_chatlog->palette().color(ui_ic_chatlog->foregroundRole()));

  auto set_format_color = [this](const QString &f_identifier, QTextCharFormat &f_format) {

    if(ao_app->current_theme->m_jsonLoaded)
    {
      if (const std::optional<QColor> l_color = ThemeManager::get().mCurrentThemeReader.getChatlogColor(f_identifier); l_color.has_value())
        f_format.setForeground(*l_color);

      if (ThemeManager::get().mCurrentThemeReader.getChatlogBool(f_identifier))
        f_format.setFontWeight(QFont::Bold);
      ;
    }
    else
    {
      if (const std::optional<QColor> l_color = ao_app->maybe_color(QString("ic_chatlog_%1_color").arg(f_identifier), COURTROOM_FONTS_INI); l_color.has_value())
        f_format.setForeground(l_color.value());
      if (ao_app->get_font_property(QString("ic_chatlog_%1_bold").arg(f_identifier), COURTROOM_FONTS_INI))
        f_format.setFontWeight(QFont::Bold);
    }



  };

  m_ic_log_format.name = m_ic_log_format.base;
  set_format_color("showname", m_ic_log_format.name);

  m_ic_log_format.selfname = m_ic_log_format.name;
  if (ao_config->log_display_self_highlight_enabled())
    set_format_color("selfname", m_ic_log_format.selfname);

  m_ic_log_format.message = m_ic_log_format.base;
  set_format_color("message", m_ic_log_format.message);

  m_ic_log_format.system = m_ic_log_format.base;
  set_format_color("system", m_ic_log_format.system);
}

void Courtroom::update_ic_log(bool p_reset_log)
{
  if (const int l_record_count = m_ic_record_list.length() + m_ic_record_queue.length(); l_record_count > ao_config->log_max_lines())
    m_ic_record_list = m_ic_record_list.mid(l_record_count - ao_config->log_max_lines());

  if (p_reset_log || ao_config->log_max_lines() == 1)
  {
    if (p_reset_log)
    {
      load_ic_text_format();
      QQueue<DRChatRecord> l_new_queue;
      dynamic_cast<QList<DRChatRecord> &>(l_new_queue) = std::move(m_ic_record_list);
      l_new_queue.append(std::move(m_ic_record_queue));
      m_ic_record_queue = std::move(l_new_queue);
    }

    ui_ic_chatlog->clear();
    ui_ic_chatlog->setAlignment(ui_ic_chatlog->get_text_alignment());
  }
  bool l_log_is_empty = m_ic_record_list.length() == 0;

  const bool l_topdown_orientation = ao_config->log_is_topdown_enabled();
  const bool l_use_newline = ao_config->log_format_use_newline_enabled();
  QTextCursor l_cursor = ui_ic_chatlog->textCursor();
  const QTextCursor::MoveOperation move_type = l_topdown_orientation ? QTextCursor::End : QTextCursor::Start;

  const QTextCharFormat &l_name_format = m_ic_log_format.name;
  const QTextCharFormat &l_selfname_format = m_ic_log_format.selfname;
  const QTextCharFormat &l_message_format = m_ic_log_format.message;
  const QTextCharFormat &l_system_format = m_ic_log_format.system;

  QScrollBar *l_scrollbar = ui_ic_chatlog->verticalScrollBar();
  const int l_scroll_pos = l_scrollbar->value();
  const bool l_is_end_scroll_pos = p_reset_log || (l_topdown_orientation ? l_scroll_pos == l_scrollbar->maximum() : l_scroll_pos == l_scrollbar->minimum());

  while (!m_ic_record_queue.isEmpty())
  {
    const DRChatRecord l_record = m_ic_record_queue.takeFirst();
    bool hideMessage = l_record.is_typed() && m_ic_record_queue.isEmpty();
    m_ic_record_list.append(l_record);

    if (!ao_config->log_display_empty_messages_enabled() && l_record.get_message().trimmed().isEmpty())
      continue;

    if (!ao_config->log_display_music_switch_enabled() && l_record.is_music())
      continue;

    l_cursor.movePosition(move_type);

    const QString l_linefeed(QChar::LineFeed);
    if (!l_log_is_empty)
      l_cursor.insertText(l_linefeed + QString(l_use_newline ? l_linefeed : nullptr), l_message_format);
    l_log_is_empty = false;

    if (!l_topdown_orientation)
      l_cursor.movePosition(move_type);

    // self-highlight check
    const QTextCharFormat &l_target_name_format = (l_record.is_self() && ao_config->log_display_self_highlight_enabled()) ? l_selfname_format : l_name_format;

    if (ao_config->log_display_timestamp_enabled())
      l_cursor.insertText(QString("[%1] ").arg(l_record.get_timestamp().toString("hh:mm")), l_target_name_format);

    if (l_record.is_system())
    {
      l_cursor.insertText(l_record.get_message(), l_system_format);
    }
    else
    {
      QString l_separator;
      if (l_use_newline)
        l_separator = QString(QChar::LineFeed);
      else if (!l_record.is_music())
        l_separator = ": ";
      else
        l_separator = " ";

      const int l_client_id = l_record.get_client_id();
      if (l_client_id != NoClientId && ao_config->log_display_client_id_enabled())
        l_cursor.insertText(QString::number(l_record.get_client_id()) + " | ", l_target_name_format);

      l_cursor.insertText(l_record.get_name() + l_separator, l_target_name_format);

      if(!hideMessage || !l_topdown_orientation)
        l_cursor.insertText(l_record.get_message(), l_message_format);
    }
  }

  { // remove unneeded blocks
    const int l_max_block_count = m_ic_record_list.length() * (1 + l_use_newline) + (l_use_newline * (m_ic_record_list.length() - 1)) + !l_topdown_orientation;
    const QTextCursor::MoveOperation l_orientation = l_topdown_orientation ? QTextCursor::Start : QTextCursor::End;
    const QTextCursor::MoveOperation l_block_orientation = l_topdown_orientation ? QTextCursor::NextBlock : QTextCursor::PreviousBlock;

    const int l_remove_block_count = ui_ic_chatlog->document()->blockCount() - l_max_block_count;
    if (l_remove_block_count > 0)
    {
      l_cursor.movePosition(l_orientation);
      for (int i = 0; i < l_remove_block_count; ++i)
        l_cursor.movePosition(l_block_orientation, QTextCursor::KeepAnchor);
      l_cursor.removeSelectedText();
    }
  }

  if (l_is_end_scroll_pos)
  {
    l_scrollbar->setValue(l_topdown_orientation ? l_scrollbar->maximum() : l_scrollbar->minimum());
  }
}

void Courtroom::on_ic_chatlog_scroll_changed()
{
  QScrollBar *l_scrollbar = ui_ic_chatlog->verticalScrollBar();
  const int l_scroll_pos = l_scrollbar->value();
  const bool l_topdown_orientation = ao_config->log_is_topdown_enabled();
  const bool l_is_end_scroll_pos = (l_topdown_orientation ? l_scroll_pos == l_scrollbar->maximum() : l_scroll_pos == l_scrollbar->minimum());

  if (l_is_end_scroll_pos)
  {
    ui_ic_chatlog_scroll_topdown->hide();
    ui_ic_chatlog_scroll_bottomup->hide();
  }
  else
  {
    if (l_topdown_orientation)
      ui_ic_chatlog_scroll_topdown->show();
    else
      ui_ic_chatlog_scroll_bottomup->show();
  }
}

void Courtroom::on_ic_chatlog_scroll_topdown_clicked()
{
  QScrollBar *l_scrollbar = ui_ic_chatlog->verticalScrollBar();
  l_scrollbar->setValue(l_scrollbar->maximum());
}

void Courtroom::on_ic_chatlog_scroll_bottomup_clicked()
{
  QScrollBar *l_scrollbar = ui_ic_chatlog->verticalScrollBar();
  l_scrollbar->setValue(l_scrollbar->minimum());
}

void Courtroom::append_ic_text(QString p_name, QString p_line, bool p_system, bool p_music, int p_client_id, bool p_self, bool typingEnabled)
{
  if (p_name.trimmed().isEmpty())
    p_name = "Anonymous";

  if (p_line.trimmed().isEmpty())
    p_line = p_line.trimmed();

  DRChatRecord new_record(p_name, p_line);
  new_record.set_system(p_system);
  new_record.set_client_id(p_client_id);
  new_record.set_self(p_self);
  new_record.set_music(p_music);
  new_record.set_typed(typingEnabled);
  m_ic_record_queue.append(new_record);
  update_ic_log(false);
}

/**
 * @brief Appends a message arriving from system to the IC chatlog.
 *
 * @param p_showname The showname used by the system. Can be an empty string.
 * @param p_line The message that the system is sending.
 */
void Courtroom::append_system_text(QString p_showname, QString p_line)
{
  if (p_line.isEmpty())
  {
    return;
  }
  append_ic_text(p_showname, p_line, true, false, NoClientId, false);
}

void Courtroom::play_preanim()
{
  // set state
  anim_state = 1;

  if (m_msg_is_first_person)
  {
    qDebug() << "[viewport] Skipping character animation as first person.";
    preanim_done();
    return;
  }

  const QString l_chr_name = m_chatmessage[CMChrName];
  const QString l_anim_name = m_chatmessage[CMPreAnim];
  qDebug() << "[viewport] Playing character animation; character:" << l_chr_name << "animation: " << l_anim_name << "file:" << ui_vp_player_char->file_name();
  ui_vp_player_char->set_play_once(true);
  ui_vp_player_char->clearImageLayers();
  ui_vp_player_pair->clearImageLayers();
  swap_viewport_reader(ui_vp_player_char, ViewportCharacterPre);
  ui_vp_player_char->start(m_ActorScaling, m_ActorScale);
}

void Courtroom::preanim_done()
{
  if (anim_state != 1)
  {
    return;
  }
  handle_chatmessage_3();

}

void Courtroom::realization_done()
{
  ui_vp_effect->stop();
}

void Courtroom::setup_chat()
{
  if(!m_appendMessage)
    ui_vp_message->clear();

  set_text_color();
  m_rainbow_step = 0;
  // we need to ensure that the text isn't already ticking because this function
  // can be called by two logic paths
  if (text_state != 0)
    return;

  if (chatmessage_is_empty)
  {
    // since the message is empty, it's technically done ticking
    text_state = 2;
    return;
  }

  ui_vp_chatbox->show();

  m_tick_speed = 0;
  m_tick_step = 0;
  is_ignore_next_letter = false;
  m_blip_step = 0;

  // Cache these so chat_tick performs better
  if(ao_app->current_theme->m_jsonLoaded)
  {
    widgetFontStruct messageFont = ThemeManager::get().mCurrentThemeReader.GetFontData(ThemeSceneType::SceneType_Courtroom, "message");
    m_chatbox_message_outline = messageFont.outline;
    m_messageOutlineColor = messageFont.outlineColor;
    m_messageOutlineSize = messageFont.outlineSize;
  }
  else
  {
    m_chatbox_message_outline = (ao_app->get_font_property("message_outline", COURTROOM_FONTS_INI) == 1);
    m_messageOutlineColor = QColor(Qt::black);
    m_messageOutlineSize = 1;
  }

  m_chatbox_message_enable_highlighting = (ao_app->current_theme->read_config_bool("enable_highlighting"));
  m_chatbox_message_highlight_colors = ao_app->get_highlight_colors();

  QString f_gender = "male";
  QString l_jsonPath = fs::characters::getFilePath(m_chatmessage[CMChrName], "char.json");
  if(FS::Checks::FileExists(l_jsonPath))
  {
    rolechat::actor::JsonActorData speakerActor;
    speakerActor.load(m_chatmessage[CMChrName].toStdString(), fs::characters::getDirectoryPath(m_chatmessage[CMChrName]).toStdString());
    f_gender = QString::fromStdString(speakerActor.gender());
  }
  else
  {
    f_gender = ao_app->get_gender(m_chatmessage[CMChrName]);
  }

  audio::blip::SetGender(f_gender.toUtf8());

  // means text is currently ticking
  text_state = 1;
}

void Courtroom::start_chat_timer()
{
  if (m_tick_timer->isActive())
  {
    return;
  }
  m_tick_timer->start();
}

void Courtroom::stop_chat_timer()
{
  m_tick_timer->stop();
}

void Courtroom::calculate_chat_tick_interval()
{
  double l_tick_rate = ao_config->chat_tick_interval();
  if (m_server_tick_rate.has_value())
    l_tick_rate = qMax(m_server_tick_rate.value(), 0);
  l_tick_rate = qBound(0.0, l_tick_rate * (1.0 - qBound(-1.0, 0.4 * m_tick_speed, 1.0)), l_tick_rate * 2.0);
  if(customMessageSpeed != 0) l_tick_rate = customMessageSpeed;
  if(is_delay_next_letter)
  {
    m_tick_timer->setInterval(l_tick_rate + ao_config->punctuation_delay() + pendingDelay);
      return;
  }
  m_tick_timer->setInterval(l_tick_rate + pendingDelay);
}

void Courtroom::next_chat_letter()
{
  if(pendingDelay != 0)
  {
    pendingDelay = 0;
    calculate_chat_tick_interval();
  }
  const QString &f_message = m_chatmessage[CMMessage];

  static const QList<QChar> punctuationCharacters = { '.', '!', '?', ',' };

  int message_length = f_message.length();

  QVector<IncomingTagData> unprocessedTags = {};
  for(IncomingTagData tag : m_ProcessedTags)
  {
    if(tag.timestamp <= m_tick_step || m_tick_step >= message_length)
    {
      switch(tag.action)
      {
      case TagType_SoundEffect:
        audio::effect::StopAll();
        audio::effect::PlayCharacter(m_chatmessage[CMChrName].toStdString(), tag.variables.at(1).toString().toStdString());
        break;

      case TagType_Flip:
        ui_vp_player_char->setMirrored(ui_vp_player_char->mirroredState() == false);
        break;

      case TagType_Hide:
        ui_vp_player_char->setVisible(ui_vp_player_char->isVisible() == false);
        break;

      case TagType_Wait:
        currentDelayLeft -= tag.variables.at(1).toInt();
        if(currentDelayLeft >= 0)
        {
          pendingDelay = tag.variables.at(1).toInt();
          calculate_chat_tick_interval();
        }
        break;

      case TagType_Speed:
        customMessageSpeed = tag.variables.at(1).toInt();
        if(customMessageSpeed >= 150) customMessageSpeed = 150;
        if(customMessageSpeed < 1) customMessageSpeed = 1;
        calculate_chat_tick_interval();
        break;

      case TagType_NewLine:
        ui_vp_message->textCursor().insertText("\n");
        break;

      case TagType_PlaySequence:
        ui_vp_player_char->setCharacterAnimation(tag.variables.at(1).toString(), m_chatmessage[CMChrName]);
        break;

      default:
        break;
      }
    }
    else
    {
      unprocessedTags.append(tag);
    }
  }

  m_ProcessedTags = unprocessedTags;
  if (m_tick_step >= message_length || ui_vp_chatbox->isHidden())
  {
    post_chatmessage();
    return;
  }

  // note: this is called fairly often(every 60 ms when char is talking)
  // do not perform heavy operations here
  QTextCharFormat vp_message_format = ui_vp_message->currentCharFormat();
  vp_message_format.setTextOutline(m_chatbox_message_outline ? QPen(m_messageOutlineColor, m_messageOutlineSize) : Qt::NoPen);

  QTextCursor cursor2 = ui_ic_chatlog->textCursor();

  auto insertChar = [&](QChar ch, const QTextCharFormat &format) {
    ui_vp_message->textCursor().insertText(ch, format);
    LuaBridge::LuaEventCall("OnMessageTick", QString(ch).toStdString());

    QScrollBar *l_scrollbar = ui_ic_chatlog->verticalScrollBar();
    const int l_scroll_pos = l_scrollbar->value();
    bool l_scroll_limt = l_scroll_pos == l_scrollbar->maximum();
    if(ao_config->log_is_topdown_enabled())
    {
      cursor2.setPosition(m_iclog_cursor_position);
      ui_ic_chatlog->setTextCursor(cursor2);
      cursor2.insertText(ch, format);
      m_iclog_cursor_position += 1;


      if(l_scroll_limt)
      {
        l_scrollbar->setValue(l_scrollbar->maximum());
      }
      else
      {
        l_scrollbar->setValue(l_scroll_pos);
      }
    }
  };

  auto advanceLetter = [&]() {
    ++m_tick_step;
    is_ignore_next_letter = false;
  };

  const QChar f_character = f_message.at(m_tick_step);

  if (punctuationCharacters.contains(f_character) && m_tick_step != message_length - 1)
  {
    if(!is_delay_next_letter)
    {
        is_delay_next_letter = true;
        calculate_chat_tick_interval();
    }
  }
  else if(is_delay_next_letter)
  {
      is_delay_next_letter = false;
      calculate_chat_tick_interval();
  }

  if(!is_ignore_next_letter)
  {

    if (f_character == Qt::Key_Backslash)
    {
      ++m_tick_step;
      is_ignore_next_letter = true;
      next_chat_letter();
      return;
    }
    else if ((f_character == Qt::Key_BraceLeft || f_character == Qt::Key_BraceRight))
    {
      ++m_tick_step;
      m_tick_speed = qBound(-3, m_tick_speed + (f_character == '}' ? 1 : -1), 3);
      calculate_chat_tick_interval();
      next_chat_letter();
      return;
    }
  }
  else
  {
    switch (f_character.unicode())
    {
    case 's':
      play_screenshake_anim();
      advanceLetter();
      next_chat_letter();
      return;

    case 'f':
      playEffect("effect_flash", "");
      advanceLetter();
      next_chat_letter();
      return;

    case 'n':
      insertChar('\n', vp_message_format);
      advanceLetter();
      next_chat_letter();
      return;

    default:
      break;
    }
  }

  if (f_character == Qt::Key_Space)
  {
    //ui_vp_message->insertPlainText(f_character);
    insertChar(' ', vp_message_format);
    //ui_ic_chatlog->insertPlainText(f_character);
  }
  else if (m_chatmessage[CMTextColor].toInt() == DR::CRainbow)
  {
    static const QStringList rainbowColors = { "#BA1518", "#D55900", "#E7CE4E", "#65C856", "#1596C8" };
    QColor color(rainbowColors[m_rainbow_step]);
    vp_message_format.setForeground(color);
    insertChar(f_character, vp_message_format);
    m_rainbow_step = (m_rainbow_step + 1) % rainbowColors.size();
  }
  else if (m_chatbox_message_enable_highlighting)
  {
    bool highlight_found = false;
    bool render_character = true;
    // render_character should only be false if the character is a highlight
    // character specifically marked as a character that should not be
    // rendered.
    if (m_message_color_stack.isEmpty())
      m_message_color_stack.push("");

    if (!is_ignore_next_letter)
    {
      for (const auto &col : qAsConst(m_chatbox_message_highlight_colors))
      {
        if (f_character == col[0][0] && m_message_color_name != col[1])
        {
          m_message_color_stack.push(col[1]);
          m_message_color_name = m_message_color_stack.top();
          render_character = (col[2] != "0");
          highlight_found = true;
          break;
        }
      }
    }

    QColor text_color = m_message_color_name.isEmpty() ? m_message_color : QColor(m_message_color_name);
    vp_message_format.setForeground(text_color);

    QString m_future_string_color = m_message_color_name;

    if (!highlight_found && !is_ignore_next_letter)
    {
      for (const auto &col : qAsConst(m_chatbox_message_highlight_colors))
      {
        if (f_character == col[0][1])
        {
          if (m_message_color_stack.size() > 1)
            m_message_color_stack.pop();
          m_future_string_color = m_message_color_stack.top();
          render_character = (col[2] != "0");
          break;
        }
      }
    }

    if (render_character)
      insertChar(f_character, vp_message_format);

    m_message_color_name = m_future_string_color;
  }
  else
  {
    insertChar(f_character, vp_message_format);
  }

  QScrollBar *scroll = ui_vp_message->verticalScrollBar();
  scroll->setValue(scroll->maximum());

  if ((f_message.at(m_tick_step) != ' ' || ao_config->blank_blips_enabled()))
  {

    int overideBlipRate = audio::blip::getBlipRate();
    overideBlipRate = overideBlipRate == -1 ? ao_config->blip_rate() : overideBlipRate;

    if (m_blip_step % overideBlipRate == 0)
    {
      m_blip_step = 0;
      if(!LuaBridge::LuaEventCall("BlipTickEvent"))
      {
        audio::blip::Tick();
        LuaBridge::LuaEventCall("OnBlipTick");
      }
    }

    ++m_blip_step;
  }
  ui_vp_message->repaint();

  ++m_tick_step;
  is_ignore_next_letter = false;
  m_tick_timer->start();
}

void Courtroom::post_chatmessage()
{
  LuaBridge::LuaEventCall("OnMessageCompleted", m_chatmessage[CMMessage].trimmed().isEmpty());
  m_tick_timer->stop();
  if (m_game_state != GameState::Preloading)
  {
    m_game_state = GameState::Finished;
  }
  text_state = 2;
  anim_state = 3;

  if (!m_hide_character && !m_msg_is_first_person)
  {
    swap_viewport_reader(ui_vp_player_char, ViewportCharacterIdle);
    ui_vp_player_char->setLayerState(ViewportCharacterIdle);
    ui_vp_player_char->start(m_ActorScaling, m_ActorScale);
  }

  m_message_color_name = "";
  m_message_color_stack.clear();

  if (ui_vp_chatbox->isVisible())
  {
    ui_vp_chat_arrow->restart();
    ui_vp_chat_arrow->show();
  }
}

void Courtroom::play_sfx()
{
  const QString l_effect = m_chatmessage[CMSoundName];
  if (l_effect.isEmpty() || l_effect == "0" || l_effect == "1")
    return;
  const QString l_chr = m_chatmessage[CMChrName];
  audio::effect::PlayCharacter(l_chr.toStdString(), l_effect.toStdString());
}

void Courtroom::on_loading_bar_delay_changed(int p_delay)
{
  m_loading_timer->setInterval(p_delay);
}

void Courtroom::set_text_color()
{
  const QMap<DR::Color, DR::ColorInfo> color_map = ao_app->get_chatmessage_colors();
  const DR::Color color = DR::Color(m_chatmessage[CMTextColor].toInt());
  const QString color_code = color_map[color_map.contains(color) ? color : DR::CDefault].code;
  ui_vp_message->setStyleSheet("background-color: rgba(0, 0, 0, 0)");
  m_message_color.setNamedColor(color_code);
}

void Courtroom::set_ban(int p_cid)
{
  if (p_cid != user::GetCharacterId() && p_cid != SpectatorId)
    return;

  call_notice(localization::getText("NOTICE_BANNED"));

  ao_app->construct_lobby();
  ao_app->destruct_courtroom();
}

void Courtroom::handle_song(QStringList p_contents)
{
  if (p_contents.size() < 4)
    return;

  QString l_song = p_contents.at(0);
  for (auto &i_extension : FS::Formats::SupportedAudio())
  {
    const QString l_fetched_song = l_song + i_extension;
    const QString l_path = ao_app->get_music_path(l_fetched_song);
    if (FS::Checks::FileExists(l_path))
    {
      l_song = l_fetched_song;
      break;
    }
  }

  const int l_chr_id = p_contents.at(1).toInt();

  QString l_showname = p_contents.at(2);

  {
    const bool l_restart = p_contents.at(3).toInt();
    if (m_current_song == l_song && !l_restart)
      return;
  }
  m_current_song = l_song;

  replays::recording::musicChange(l_song);
  DRAudiotrackMetadata l_song_meta(l_song);

  if(!LuaBridge::SongChangeEvent(l_song.toStdString(), l_song_meta.title().toStdString(), l_showname.toStdString()))
  {
    if(p_contents.length() > 4)
    {
      audio::bgm::PlayMode(l_song.toStdString(), BGMPlayback(p_contents.at(4).toInt()));
    }


    if (l_chr_id >= 0 && l_chr_id < CharacterRepository::serverListLength())
    {
      if (l_showname.isEmpty())
      {
        l_showname = ao_app->get_showname(CharacterRepository::characterNameServer(l_chr_id));
      }

      append_ic_text(l_showname, "has played a song: " + l_song_meta.title(), false, true, NoClientId, l_chr_id == user::GetCharacterId());

      if (ao_config->log_is_recording_enabled())
      {
        save_textlog(l_showname + " has played a song: " + l_song_meta.filename());
      }
    }

    set_music_text(l_song_meta.title());

    LuaBridge::OnSongChange(l_song.toStdString(), l_song_meta.title().toStdString(), l_showname.toStdString());
  }


}

void Courtroom::handle_wtce(QString p_wtce)
{
  int index = p_wtce.at(p_wtce.size() - 1).digitValue();
  if (index > 0 && index < wtce_names.size() + 1 && wtce_names.size() > 0) // check to prevent crash
  {
    p_wtce.chop(1); // looking for the 'testimony' part
    if (p_wtce == "testimony")
    {
      audio::effect::Play(ao_app->get_sfx(wtce_names[index - 1]).toStdString());
      replays::recording::splashAnimation(wtce_names[index - 1]);
      ui_vp_wtce->play(wtce_names[index - 1]);
    }
  }
}

void Courtroom::set_hp_bar(int p_bar, int p_state)
{
  if (p_state < 0 || p_state > 10) return;
  if (p_bar == 1) ui_defense_bar->SetValue(p_state);
  else if (p_bar == 2) ui_prosecution_bar->SetValue(p_state);
}

void Courtroom::set_character_position(QString p_pos)
{
  const bool l_is_default_pos = p_pos.toStdString() == engine::actor::user::retrieve()->side();

  int l_pos_index = ui_pos_dropdown->currentIndex();
  if (!l_is_default_pos)
  {
    const int l_new_pos_index = ui_pos_dropdown->findData(p_pos);
    if (l_new_pos_index != -1)
    {
      l_pos_index = l_new_pos_index;
    }
  }
  ui_pos_dropdown->setCurrentIndex(l_pos_index);

  // enable judge mechanics if appropriate
  set_judge_enabled(p_pos == "jud");
}

/**
 * @brief Send a OOC packet (CT) out to the server.
 * @param ooc_name The username.
 * @param ooc_message The message.
 */
void Courtroom::send_ooc_packet(QString ooc_message)
{
  while (ao_config->username().isEmpty())
  {
    QInputDialog l_dialog;
    l_dialog.setWindowTitle(tr("Enter a name"));
    l_dialog.setLabelText(tr("You must have a username to talk in OOC chat."));
    if (l_dialog.exec())
    {
      ao_config->set_username(l_dialog.textValue());
    }
    else
    {
      return;
    }
  }

  if (ooc_message.trimmed().isEmpty())
  {
    append_server_chatmessage("CLIENT",  localization::getText("OOC_EMPTY"));
    return;
  }
  QStringList l_content{ao_config->username(), ooc_message};
  ao_app->send_server_packet(DRPacket("CT", l_content));
}

void Courtroom::mod_called(QString p_ip)
{
  ui_ooc_chatlog->append(p_ip);
  if (ao_config->server_alerts_enabled())
  {
    audio::system::Play(ao_app->get_sfx("mod_call").toUtf8());
    ao_app->alert(this);
    if (ao_config->log_is_recording_enabled())
      save_textlog("(OOC)(MOD CALL)" + p_ip);
  }
}

void Courtroom::on_ic_showname_editing_finished()
{
  const QString l_text = ui_ic_chat_showname->text().simplified();
  ui_ic_chat_showname->setText(l_text);
  set_showname(l_text);
}

void Courtroom::set_showname(QString p_showname)
{
  ao_config->set_showname(p_showname);
}

void Courtroom::on_ooc_name_editing_finished()
{
  const QString l_text = ui_ooc_chat_name->text().simplified();
  ui_ooc_chat_name->setText(l_text);
  ao_config->set_username(l_text);
}

void Courtroom::on_ooc_message_return_pressed()
{
  LuaBridge::LuaEventCall("OnOOCMessageSend");
  const QString l_message = ui_ooc_chat_message->text();
  if (l_message.isEmpty())
    return;

  LuaBridge::LuaEventCall("OnOOCMessageSend");

  if (l_message.startsWith("/rainbow") && !is_rainbow_enabled)
  {
    ui_text_color->addItem(localization::getText("COLOR_RAINBOW"));
    ui_ooc_chat_message->clear();
    is_rainbow_enabled = true;
    return;
  }
  if (l_message.startsWith("/afk") && !m_isAfk)
  {
    m_lastActivityTimestamp = 1;
    m_isAfk = true;
    ao_app->send_server_packet(DRPacket("STATUS", {QString::number(UserState_AFK), QString::number(true)}));
    ui_ooc_chat_message->clear();
    return;
  }
  else if (l_message.startsWith("/switch_am"))
  {
    on_switch_area_music_clicked();
    ui_ooc_chat_message->clear();
    return;
  }
  else if (l_message.startsWith("/rollp"))
  {
    audio::effect::Play(ao_app->get_sfx("dice").toStdString());
  }
  else if (l_message.startsWith("/roll"))
  {
    audio::effect::Play(ao_app->get_sfx("dice").toStdString());
  }
  else if (l_message.startsWith("/coinflip"))
  {
    audio::effect::Play(ao_app->get_sfx("coinflip").toStdString());
  }
  else if (l_message.startsWith("/tr "))
  {
    // Timer resume
    int space_location = l_message.indexOf(" ");

    int timer_id;
    if (space_location == -1)
      timer_id = 0;
    else
      timer_id = l_message.mid(space_location + 1).toInt();
    resume_timer(timer_id);
  }
  else if (l_message.startsWith("/ts "))
  {
    // Timer set
    QStringList arguments = l_message.split(" ");
    int size = arguments.size();

    // Note arguments[0] == "/ts", so every index (and thus length) is off by
    // one.
    if (size > 5)
      return;

    int timer_id = (size > 1 ? arguments[1].toInt() : 0);
    int new_time = (size > 2 ? arguments[2].toInt() : 300) * 1000;
    int timestep_length = (size > 3 ? arguments[3].toDouble() : -.016) * 1000;
    int firing_interval = (size > 4 ? arguments[4].toDouble() : .016) * 1000;
    set_timer_time(timer_id, new_time);
    set_timer_timestep(timer_id, timestep_length);
    set_timer_firing(timer_id, firing_interval);
  }
  else if (l_message.startsWith("/tp "))
  {
    // Timer pause
    int space_location = l_message.indexOf(" ");

    int timer_id;
    if (space_location == -1)
      timer_id = 0;
    else
      timer_id = l_message.mid(space_location + 1).toInt();
    pause_timer(timer_id);
  }
  else if(l_message.startsWith("/"))
  {
    QStringList commandArguments = l_message.mid(1).split(" ");
    if(commandArguments.at(0).length() > 0)
    {
      QString commandFunction = "OnCommand_" + commandArguments.at(0).toLower();
      if(!commandFunction.isEmpty())
      {
        if(LuaBridge::LuaEventCall(commandFunction.toUtf8()))
        {
          ui_ooc_chat_message->clear();
          return;
        }
      }
    }
  }

  send_ooc_packet(l_message);
  ui_ooc_chat_message->clear();

  ui_ooc_chat_message->setFocus();
}

void Courtroom::on_pos_dropdown_changed()
{
  const QString l_pos = get_current_position();
  send_ooc_packet("/pos " + l_pos);
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_area_list_clicked()
{
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_area_list_double_clicked(QModelIndex p_model)
{
  const QString l_area_name = ui_area_list->item(p_model.row())->text();
  ao_app->send_server_packet(DRPacket("MC", {l_area_name, QString::number(user::GetCharacterId())}));
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_area_search_edited(QString p_filter)
{
  ui_area_list->filterList(p_filter);
}

void Courtroom::on_area_search_edited()
{
  on_area_search_edited(ui_area_search->text());
}

void Courtroom::on_music_list_clicked()
{
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_music_list_double_clicked(QModelIndex p_model)
{
  const QString l_song_name = ui_music_list->item(p_model.row())->data(Qt::UserRole).toString();
  send_mc_packet(l_song_name);
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_music_menu_play_triggered()
{
  QListWidgetItem *l_item = ui_music_list->currentItem();
  if (l_item)
  {
    const QString l_song = l_item->data(Qt::UserRole).toString();
    send_mc_packet(l_song);
  }
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_music_menu_insert_ooc_triggered()
{
  QListWidgetItem *l_item = ui_music_list->currentItem();
  if (l_item)
  {
    const QString l_song = l_item->data(Qt::UserRole).toString();
    ui_ooc_chat_message->insert(l_song);
  }
  ui_ooc_chat_message->setFocus();
}

void Courtroom::on_music_search_edited(QString p_filter)
{
  ui_music_list->filterList(p_filter);
}

void Courtroom::on_music_search_edited()
{
  on_music_search_edited(ui_music_search->text());
}

void Courtroom::send_mc_packet(QString p_song, BGMPlayback playbackType)
{
  if (is_client_muted)
    return;

  QStringList contents = {p_song, QString::number(user::GetCharacterId())};
  if(network::metadata::VNServerInformation::featureSupported("outfits"))
  {
    contents.append(QString::number(playbackType));
  }

  ao_app->send_server_packet(DRPacket("MC", contents));
}

/**
 * @brief Set the sprites of the shout buttons, and mark the currently
 * selected shout as such.
 *
 * @details If a sprite cannot be found for a shout button, a regular
 * push button is displayed for it with its shout name instead.
 */
void Courtroom::reset_shout_buttons()
{
  for (RPButton *i_button : qAsConst(ui_shouts))
    i_button->setChecked(false);
  m_shout_state = 0;
}

void Courtroom::on_shout_button_clicked(const bool p_checked)
{
  RPButton *l_button = dynamic_cast<RPButton *>(sender());
  if (l_button == nullptr)
    return;

  bool l_ok = false;
  const int l_id = l_button->property("shout_id").toInt(&l_ok);
  if (!l_ok)
    return;

  // disable all other buttons
  for (RPButton *i_button : qAsConst(ui_shouts))
  {
    if (i_button == l_button)
      continue;
    i_button->setChecked(false);
  }
  m_shout_state = p_checked ? l_id : 0;

  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_shout_button_toggled(const bool p_checked)
{
  RPButton *l_button = dynamic_cast<RPButton *>(sender());
  if (l_button == nullptr)
    return;

  const QString l_name = l_button->property("shout_name").toString();
  if (l_name.isEmpty())
    return;

  const QString l_image_name(QString("%1%2.png").arg(l_name, QString(p_checked ? "_selected" : nullptr)));
  l_button->set_image(l_image_name);
  if (!l_button->has_image())
    l_button->setText(l_name);
}

void Courtroom::on_cycle_clicked()
{
  RPButton *f_cycle_button = static_cast<RPButton *>(sender());
  int f_cycle_id = f_cycle_button->property("cycle_id").toInt();

  switch (f_cycle_id)
  {
  case 5:
    cycle_wtce(-1);
    break;
  case 4:
    cycle_wtce(1);
    break;
  case 3:
    cycle_effect(-1);
    break;
  case 2:
    cycle_effect(1);
    break;
  case 1:
    cycle_shout(-1);
    break;
  case 0:
    cycle_shout(1);
    break;
  default:
    break;
  }

  if (ao_app->current_theme->read_config_bool("enable_cycle_ding"))
    audio::system::Play(ao_app->get_sfx("cycle").toUtf8());

  set_shouts();
  ui_ic_chat_message_field->setFocus();
}

/**
 * @brief Selects the shout p_delta slots ahead of the current one, wrapping
 * around if needed. If p_delta is negative, look -p_delta slots behind.
 * @param Shout slots to advance. May be negative.
 */
void Courtroom::cycle_shout(int p_delta)
{
  int n = ui_shouts.size();
  m_shout_current = (m_shout_current - p_delta + n) % n;
  set_shouts();
}

/**
 * @brief Selects the effect p_delta slots ahead of the current one, wrapping
 * around if needed. If p_delta is negative, look -p_delta slots behind.
 * @param Shout slots to advance. May be negative.
 */
void Courtroom::cycle_effect(int p_delta)
{
  int n = ui_effects.size();
  m_effect_current = (m_effect_current - p_delta + n) % n;
  set_effects();
}

/**
 * @brief Selects the splash p_delta slots ahead of the current one, wrapping
 * around if needed. If p_delta is negative, look -p_delta slots behind.
 * @param Shout slots to advance. May be negative.
 */
void Courtroom::cycle_wtce(int p_delta)
{
  int n = ui_wtce.size();
  m_wtce_current = (m_wtce_current - p_delta + n) % n;
  set_judge_wtce();
}

/**
 * @brief Set the sprites of the effect buttons, and mark the currently
 * selected effect as such.
 *
 * @details If a sprite cannot be found for an effect button, a regular
 * push button is displayed for it with its effect name instead.
 */
void Courtroom::reset_effect_buttons()
{
  for (RPButton *i_button : qAsConst(ui_effects))
    i_button->setChecked(false);
  m_effect_state = 0;
}

void Courtroom::on_effect_button_clicked(const bool p_checked)
{
  RPButton *l_button = dynamic_cast<RPButton *>(sender());
  if (l_button == nullptr)
    return;

  bool l_ok = false;
  const int l_id = l_button->property("effect_id").toInt(&l_ok);
  if (!l_ok)
    return;

  // disable all other buttons
  for (RPButton *i_button : qAsConst(ui_effects))
  {
    if (i_button == l_button)
      continue;
    i_button->setChecked(false);
  }

  m_effect_state = p_checked ? l_id : 0;
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_effect_button_toggled(const bool p_checked)
{
  RPButton *l_button = dynamic_cast<RPButton *>(sender());
  if (l_button == nullptr)
    return;

  const QString l_name = l_button->property("effect_name").toString();
  if (l_name.isEmpty())
    return;

  const QString l_image_name(QString("%1%2.png").arg(l_name, QString(p_checked ? "_pressed" : nullptr)));
  l_button->set_image(l_image_name);
  if (!l_button->has_image())
    l_button->setText(l_name);
}

void Courtroom::on_text_color_changed(int p_color)
{
  m_text_color = p_color;
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_chat_type_changed(int p_type)
{
  m_current_chat_type = static_cast<ChatTypes>(p_type);
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::onOutfitChanged(int outfitIndex)
{
  if(outfitIndex == -1) return;
  int trueOutfitIndex = outfitIndex -1;
  if(engine::actor::user::retrieve() == nullptr) return;
  if(trueOutfitIndex == -1)
  {
    engine::actor::user::retrieve()->switchOutfit("<All>");
  }
  else if(engine::actor::user::retrieve()->outfitNames().size() > trueOutfitIndex && trueOutfitIndex != -1)
  {
    engine::actor::user::retrieve()->switchOutfit(engine::actor::user::retrieve()->outfitNames()[trueOutfitIndex]);
  }

  ui_emotes->refreshSelection(false);
  ui_emotes->refreshEmotes(false);
  onAnimListItemChanged(nullptr, nullptr);
  ui_emotes->outfitChange();

  const QString l_chr_name = get_character_ini();
  const QString l_showname = QString::fromStdString(engine::actor::user::retrieve()->showname());
  const QString l_final_showname = l_showname.trimmed().isEmpty() ? l_chr_name : l_showname;

  ao_config->set_showname_placeholder(l_final_showname);
  QStringList l_content{l_chr_name, l_final_showname};
  if(network::metadata::VNServerInformation::featureSupported("outfits")) l_content.append(QString::fromStdString(engine::actor::user::retrieve()->outfit()));

  ao_app->send_server_packet(DRPacket("chrini", l_content));
}

/**
 * @brief Set the sprites of the splash buttons.
 *
 * @details If a sprite cannot be found for a shout button, a regular
 * push button is displayed for it with its shout name instead.
 */
void Courtroom::reset_wtce_buttons()
{
  for (int i = 0; i < wtce_names.size(); ++i)
  {
    const QString l_name = wtce_names.at(i);
    const QString l_file = l_name + ".png";
    RPButton *l_button = ui_wtce.at(i);
    l_button->set_image(l_file);
    l_button->setText(!l_button->has_image() ? l_name : nullptr);
  }

  m_wtce_current = 0;

  // Unlike the other reset functions, the judge buttons are of immediate
  // action and thus are immediately unpressed after being pressed.
  // Therefore, we do not need to handle displaying a "_selected.png"
  // when appropriate, because there is no appropriate situation
}

void Courtroom::on_wtce_clicked()
{
  //  qDebug() << "AA: wtce clicked!";
  if (is_client_muted)
    return;

  RPButton *f_sig = static_cast<RPButton *>(sender());
  QString id = f_sig->property("wtce_id").toString();

  ao_app->send_server_packet(DRPacket("RT", {QString("testimony%1").arg(id)}));

  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_change_character_clicked()
{
  audio::Suppress(true);

  set_char_select();
  set_char_select_page();

  ui_spectator->show();

  ao_app->send_server_packet(DRPacket("CharsCheck"));
}

void Courtroom::load_theme()
{
  ao_app->current_theme->InitTheme();
  switchToggle("all");
  setup_courtroom();
  update_background_scene();

  if (ao_app->current_theme->read_config_bool("use_toggles")) ThemeManager::get().resetSelectedTabs();
}

void Courtroom::reload_theme()
{

  if(ThemeManager::get().getReloadPending())
  {
    m_shout_state = 0;
    m_shout_current = 0;
    check_shouts();
    if (m_shout_current < shouts_enabled.length() && !shouts_enabled[m_shout_current])
      cycle_shout(1);

    m_effect_state = 0;
    m_effect_current = 0;
    check_effects();
    if (m_effect_current < effects_enabled.length() && !effects_enabled[m_effect_current])
      cycle_effect(1);

    m_wtce_current = 0;
    reset_wtce_buttons();
    check_wtce();
    if (is_judge && (m_wtce_current < wtce_enabled.length() && !wtce_enabled[m_wtce_current]))
      cycle_wtce(1);
  }

  if (m_game_state == GameState::Preloading || ui_vp_objection->is_running())
  {
    m_shout_reload_theme = true;
    return;
  }
  load_theme();
  construct_playerlist_layout();
}

void Courtroom::load_character()
{
  update_iniswap_list();
  enter_courtroom(user::GetCharacterId());
}

void Courtroom::load_audiotracks()
{
  DRAudiotrackMetadata::update_cache();
  list_music();
}

void Courtroom::on_back_to_lobby_clicked()
{
  if (m_back_to_lobby_clicked)
    return;
  m_back_to_lobby_clicked = true;

  // hide so we don't get the 'disconnected from server' prompt
  hide();
  ao_app->leave_server();
  ao_app->construct_lobby();
  ao_app->destruct_courtroom();
}

void Courtroom::on_spectator_clicked()
{
  ao_app->send_server_packet(DRPacket("CC", {QString::number(user::getOutgoingClientId()), "-1", "HDID"}));
}

void Courtroom::OnCharRefreshClicked()
{
  ao_app->reload_packages();
  pCharaSelectSeries->clear();
  pCharaSelectSeries->addItems(CharacterRepository::filterList());
  m_current_chr_page = 0;
  set_char_select_page();
}

void Courtroom::OnCharRandomClicked()
{
  int n_real_char = 0;
  std::srand(std::time(nullptr));

  QVector<ActorSelectEntry> randomList = CharacterRepository::currentList();

  int randomCount = randomList.length();

  int randomIndex = std::rand() % randomCount;

  ActorSelectEntry selectedChar = randomList[randomIndex];


  if (user::GetCharacterName().toStdString() == selectedChar.name)
  {
    enter_courtroom(user::GetCharacterId());
    return;
  }

  QString char_json_path = fs::characters::getFilePath(QString::fromStdString(selectedChar.name), CHARACTER_CHAR_JSON);
  QString char_ini_path = fs::characters::getFilePath(QString::fromStdString(selectedChar.name), CHARACTER_CHAR_INI);

  if (!FS::Checks::FileExists(char_json_path))
  {
    if (!FS::Checks::FileExists(char_ini_path))
    {
      qDebug() << "did not find " << char_ini_path;
      call_notice("Could not find " + char_ini_path);
      return;
    }
  }

  if(!CharacterRepository::characterExists(QString::fromStdString(selectedChar.name)))
  {
    n_real_char = CharacterRepository::findAvailablePersona();
    if(n_real_char == -1) return;
    ao_config->set_character_ini(CharacterRepository::characterNameServer(n_real_char), QString::fromStdString(selectedChar.name));
  }
  else
  {
    n_real_char = CharacterRepository::networkedIdFromName(QString::fromStdString(selectedChar.name));
  }

  ao_app->send_server_packet(
      DRPacket("CC", {QString::number(user::getOutgoingClientId()), QString::number(n_real_char), "HDID"}));
}

void Courtroom::SwitchRandomCharacter(QString list)
{
  int n_real_char = 0;
  std::srand(std::time(nullptr));

  QVector<ActorSelectEntry> randomList = CharacterRepository::filteredList(list);
  if(randomList.isEmpty()) return;

  int randomCount = randomList.length();

  int randomIndex = std::rand() % randomCount;

  ActorSelectEntry selectedChar = randomList[randomIndex];

  if (user::GetCharacterName().toStdString() == selectedChar.name)
  {
    enter_courtroom(user::GetCharacterId());
    return;
  }

  QString char_json_path = fs::characters::getFilePath(QString::fromStdString(selectedChar.name), CHARACTER_CHAR_JSON);
  QString char_ini_path = fs::characters::getFilePath(QString::fromStdString(selectedChar.name), CHARACTER_CHAR_INI);

  if (!FS::Checks::FileExists(char_json_path))
  {
    if (!FS::Checks::FileExists(char_ini_path))
    {
      qDebug() << "did not find " << char_ini_path;
      call_notice("Could not find " + char_ini_path);
      return;
    }
  }

  if(!CharacterRepository::characterExists(QString::fromStdString(selectedChar.name)))
  {
    n_real_char = CharacterRepository::findAvailablePersona();
    if(n_real_char == -1) return;
    ao_config->set_character_ini(CharacterRepository::characterNameServer(n_real_char), QString::fromStdString(selectedChar.name));
  }
  else
  {
    n_real_char = CharacterRepository::networkedIdFromName(QString::fromStdString(selectedChar.name));
  }

  ao_app->send_server_packet(
      DRPacket("CC", {QString::number(user::getOutgoingClientId()), QString::number(n_real_char), "HDID"}));
}

void Courtroom::on_call_mod_clicked()
{
  QMessageBox::StandardButton reply;
  QString warning = "Are you sure you want to call a mod?\n"
                    "\n"
                    "Be prepared to explain what is happening and why they are needed when they answer.";
  reply = QMessageBox::warning(this, "Warning", warning, QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

  if (reply == QMessageBox::Yes)
  {
    ao_app->send_server_packet(DRPacket("ZZ"));
    qDebug() << "Called mod";
  }
  else
    qDebug() << "Did not call mod";

  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_switch_area_music_clicked()
{
  if (is_area_music_list_separated())
    return;

  if (ui_area_list->isHidden())
  {
    ui_area_list->show();
    ui_area_search->show();
    ui_music_list->hide();
    ui_music_search->hide();
  }
  else
  {
    ui_area_list->hide();
    ui_area_search->hide();
    ui_music_list->show();
    ui_music_search->show();
  }
}

void Courtroom::on_pre_clicked()
{
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_flip_clicked()
{
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_hidden_clicked()
{
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_config_panel_clicked()
{
  ao_app->toggle_config_panel();
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::switchToggle(QString t_tabName)
{
    ThemeManager::get().toggleTab(t_tabName, "default");
    set_judge_enabled(is_judge);
}

bool Courtroom::ui_in_current_toggle(QString p_ui_name)
{
    if(m_toggle_state == ToggleState::All) return true;
    QString state_name = "";
    switch(m_toggle_state)
    {
    case ToggleState::Chat:
        state_name = "chat";
        break;

    case ToggleState::GM:
        state_name = "gm";
        break;

    case ToggleState::Area:
        state_name = "area";
        break;

    default:
        state_name = "unknown";
        break;

    };

    if(widget_toggles.contains(p_ui_name))
    {
        if(widget_toggles[p_ui_name].toLower() != state_name)
        {
            return false;
        }
    }

  return true;
}

void Courtroom::on_note_button_clicked()
{
  if (!is_note_shown)
  {
    load_note();
    ui_vp_notepad_image->show();
    ui_vp_notepad->show();
    ui_vp_notepad->setFocus();
    is_note_shown = true;
  }
  else
  {
    save_note();
    ui_vp_notepad_image->hide();
    ui_vp_notepad->hide();
    ui_ic_chat_message_field->setFocus();
    is_note_shown = false;
  }
}

void Courtroom::on_note_text_changed()
{
  ao_app->write_note(ui_vp_notepad->toPlainText(), current_file);
}

void Courtroom::ping_server()
{
  ao_app->send_server_packet(DRPacket("CH", {QString::number(user::GetCharacterId())}));
}

void Courtroom::changeEvent(QEvent *event)
{
  QWidget::changeEvent(event);
  if (event->type() == QEvent::WindowStateChange)
  {
    m_is_maximized = windowState().testFlag(Qt::WindowMaximized);
    if (!m_is_maximized)
      resize(m_default_size);
  }
}

void Courtroom::closeEvent(QCloseEvent *event)
{
  QWidget::closeEvent(event);
  Q_EMIT closing();
}

bool Courtroom::event(QEvent *event)
{
  switch (event->type()) {
  case QEvent::MouseMove:
  case QEvent::KeyPress:
  case QEvent::MouseButtonPress:
  case QEvent::MouseButtonRelease:
  case QEvent::Wheel:
    resetAFKTimer();
    break;

  case QEvent::WindowActivate:
    RuntimeLoop::setWindowFocus(true);
    break;

  case QEvent::WindowDeactivate:
    if(ao_config->focus_performance_mode_enabled()) RuntimeLoop::setWindowFocus(false);
    break;

  default:
    break;
  }

  return QWidget::event(event);
}

void Courtroom::resetAFKTimer()
{
  if (!network::metadata::VNServerInformation::featureSupported("sequence")) return;
  if (m_isAfk && (RuntimeLoop::uptime() - m_lastActivityTimestamp) < m_afkThresholdMs)
  {
    m_isAfk = false;
    ao_app->send_server_packet(DRPacket("STATUS", {QString::number(UserState_AFK), QString::number(false)}));
  }
  m_lastActivityTimestamp = RuntimeLoop::uptime();
}

void Courtroom::checkAFKStatus()
{
  if (!network::metadata::VNServerInformation::featureSupported("sequence")) return;
  if (!m_isAfk && (RuntimeLoop::uptime() - m_lastActivityTimestamp) >= m_afkThresholdMs) {
    m_isAfk = true;

    ao_app->send_server_packet(DRPacket("STATUS", {QString::number(UserState_AFK), QString::number(true)}));
  }
}

void Courtroom::keyPressEvent(QKeyEvent *event)
{
  if (!event || event->isAutoRepeat())
    return;

  int key = event->key();
  QString keyText = QKeySequence(key).toString();
  LuaBridge::LuaEventCall("OnKeyPressed", keyText.toStdString(), key);
}

void Courtroom::keyReleaseEvent(QKeyEvent *event)
{
  if (!event || event->isAutoRepeat())
    return;

  int key = event->key();
  QString keyText = QKeySequence(key).toString();
  LuaBridge::LuaEventCall("OnKeyReleased", keyText.toStdString(), key);
}

void Courtroom::mousePressEvent(QMouseEvent *event)
{
  if (event)
  {
    LuaBridge::LuaEventCall("OnMousePressed", event->button(), event->x(), event->y());
  }
}

void Courtroom::mouseReleaseEvent(QMouseEvent *event)
{
  if (event)
  {
    LuaBridge::LuaEventCall("OnMouseReleased", event->button(), event->x(), event->y());
  }
}

void Courtroom::mouseMoveEvent(QMouseEvent *event)
{
  if (event)
  {
    LuaBridge::LuaEventCall("OnMouseMoved", event->x(), event->y());
  }
}

void Courtroom::wheelEvent(QWheelEvent *event)
{
  if (event)
  {
    QPoint delta = event->angleDelta();
    LuaBridge::LuaEventCall("OnMouseWheel", delta.x(), delta.y());
  }
}

void Courtroom::focusInEvent(QFocusEvent *event)
{
  if (event)
  {
    LuaBridge::LuaEventCall("OnWindowFocusChange", true);
    RuntimeLoop::setWindowFocus(true);
  }
}

void Courtroom::focusOutEvent(QFocusEvent *event)
{
  if (event)
  {
    LuaBridge::LuaEventCall("OnWindowFocusChange", false);
    RuntimeLoop::setWindowFocus(false);
  }

}

void Courtroom::resizeEvent(QResizeEvent *event)
{
  if (event)
  {
    QSize size = event->size();
    LuaBridge::LuaEventCall("OnWindowResized", size.width(), size.height());
  }
}

void Courtroom::moveEvent(QMoveEvent *event)
{
  if (event)
  {
    QPoint pos = event->pos();
    LuaBridge::LuaEventCall("OnWindowMoved", pos.x(), pos.y());
  }
}

void Courtroom::on_set_notes_clicked()
{
  if (ui_note_scroll_area->isHidden())
    ui_note_scroll_area->show();
  else
    ui_note_scroll_area->hide();
}

void Courtroom::resume_timer(int p_id)
{
  if (p_id < 0 || p_id >= ui_timers.length())
    return;
  AOTimer *l_timer = ui_timers.at(p_id);
  l_timer->resume();
}

void Courtroom::set_timer_time(int p_id, int new_time)
{
  if (p_id < 0 || p_id >= ui_timers.length())
    return;
  AOTimer *l_timer = ui_timers.at(p_id);
  l_timer->set_time(QTime(0, 0).addMSecs(new_time));
}

void Courtroom::set_timer_timestep(int p_id, int timestep_length)
{
  if (p_id < 0 || p_id >= ui_timers.length())
    return;
  AOTimer *l_timer = ui_timers.at(p_id);
  l_timer->set_timestep_length(timestep_length);
}

void Courtroom::set_timer_firing(int p_id, int firing_interval)
{
  if (p_id < 0 || p_id >= ui_timers.length())
    return;
  AOTimer *l_timer = ui_timers.at(p_id);
  l_timer->set_firing_interval(firing_interval);
}

void Courtroom::pause_timer(int p_id)
{
  if (p_id < 0 || p_id >= ui_timers.length())
    return;
  AOTimer *l_timer = ui_timers.at(p_id);
  l_timer->pause();
}

void Courtroom::construct_playerlist()
{
  ui_player_list = new QWidget(this);
  construct_playerlist_layout();
}

void Courtroom::construct_playerlist_layout()
{
  //Clear Player List Entries
  while (!m_player_list.isEmpty())
    delete m_player_list.takeLast();

  //Setup Player list
  QPoint f_spacing = ao_app->current_theme->get_widget_settings_spacing("player_list", "courtroom", "player_list_spacing");

  engine::system::theme::applyDimensions(ui_player_list, "player_list", ThemeSceneType::SceneType_Courtroom);
  float resize = ThemeManager::get().getResize();

  int player_height = engine::system::theme::getDimensions("player_list_slot", ThemeSceneType::SceneType_Courtroom).height;
  if(player_height == 0) player_height = (int)((float)50 * resize);

  int y_spacing = f_spacing.y();
  int max_pages = ceil((SceneManager::get().mPlayerDataList.count() - 1) / m_page_max_player_count);

  player_columns = (( (int)((float)ui_player_list->height() * resize) - player_height) / (y_spacing + player_height)) + 1;

  if(m_current_reportcard_reason != ReportCardReason::None && user::GetCharacterId() != SpectatorId)
  {
    m_page_player_list = 0;
    ui_player_list_right->hide();
    ui_player_list_left->hide();
    DrPlayerListEntry* prompt_reason = new DrPlayerListEntry(ui_player_list, ao_app, 1, 0);

    prompt_reason->show();

    m_player_list.append(prompt_reason);
    switch(m_current_reportcard_reason)
    {
      case ReportCardReason::Blackout:
      prompt_reason->set_reason(localization::getText("REASON_BLACKOUT"));
        break;

      case ReportCardReason::Blinded:
        prompt_reason->set_reason(localization::getText("REASON_BLINDED"));
        break;

      case ReportCardReason::PendingLook:
        prompt_reason->set_reason(localization::getText("REASON_PENDING"));
        break;

      case ReportCardReason::NoPlayerList:
        prompt_reason->set_reason(localization::getText("REASON_DISABLED"));
        break;

      default:
        prompt_reason->set_reason("Unimplemented ReportCardReason, please make sure you're up to date.");
        break;
    }


    return;
  }
  m_page_max_player_count = qMax(1, player_columns);

  //Manage Arrows (Right)
  ui_player_list_right->hide();
  if(m_page_player_list < max_pages)
  {
    ui_player_list_right->show();
  }
  else if(m_page_player_list > max_pages)
  {
    m_page_player_list = max_pages;
  }

  //Manage Arrows (Left)
  if(m_page_player_list <= 0)
  {
    m_page_player_list = 0;
    ui_player_list_left->hide();
  }
  else ui_player_list_left->show();


  int starting_index = (m_page_player_list * m_page_max_player_count);

  int last_entry_height = 0;
  for (int n = starting_index; n < SceneManager::get().mPlayerDataList.count(); ++n)
  {
    int y_pos = (last_entry_height + y_spacing) * (n - starting_index);
    DrPlayerListEntry* ui_playername = new DrPlayerListEntry(ui_player_list, ao_app, 1, y_pos);
    last_entry_height = ui_playername->height();

    DrPlayer playerData = SceneManager::get().mPlayerDataList.at(n);
    ui_playername->set_character(playerData.m_character, playerData.m_Afk);
    ui_playername->set_name(playerData.m_showname);
    ui_playername->setURL(playerData.mURL);
    ui_playername->setID(playerData.m_id);
    ui_playername->setStatus(playerData.mPlayerStatus);
    ui_playername->setOutfit(playerData.m_CharacterOutfit);
    ui_playername->setDiscord(playerData.m_discord);

    ui_playername->setMod(playerData.mIPID, playerData.mHDID);

    m_player_list.append(ui_playername);
    ui_playername->show();

    if(n == (starting_index + m_page_max_player_count)) break;
  }
}

void Courtroom::setPlayerTyping(int client, bool active)
{
  for(DrPlayerListEntry* player : m_player_list)
  {
    if(player->clientId() == client)
    {
      player->toggleTyping(active);
      return;
    }
  }
}

void Courtroom::on_player_list_left_clicked()
{
    --m_page_player_list;

    construct_playerlist_layout();

    ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_player_list_right_clicked()
{
    ++m_page_player_list;

    construct_playerlist_layout();

    ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_area_look_clicked()
{
  LuaBridge::LuaEventCall("OnAreaLook", m_current_reportcard_reason);
  if(m_current_reportcard_reason == ReportCardReason::PendingLook)
  {
    m_current_reportcard_reason = ReportCardReason::None;
    construct_playerlist_layout();
  }
}

void Courtroom::write_area_desc()
{
  std::optional<QColor> l_color;
  bool is_bold = false;

  if(ao_app->current_theme->m_jsonLoaded)
  {
    widgetFontStruct fontstruct = ThemeManager::get().mCurrentThemeReader.GetFontData(ThemeSceneType::SceneType_Courtroom, "area_desc");
    l_color = fontstruct.color;
    is_bold = fontstruct.bold;
  }
  else
  {
    l_color = ao_app->maybe_color(QString("area_desc_color"), COURTROOM_FONTS_INI);
    is_bold = ao_app->get_font_property(QString("area_desc_bold"), COURTROOM_FONTS_INI);
  }

  ui_area_desc->ensurePolished();
  QTextCharFormat formatting = QTextCharFormat();

  //Set the color

  if(l_color.has_value())
  {
    formatting.setForeground(l_color.value());
  }

  //Set the bold
  if (is_bold) formatting.setFontWeight(QFont::Bold);

  //Clear the text
  ui_area_desc->clear();
  QTextCursor l_cursor = ui_area_desc->textCursor();
  l_cursor.insertText(m_area_description, formatting);
}
