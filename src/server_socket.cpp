#include "aoapplication.h"

#include <QDebug>

#include "aoconfig.h"
#include "courtroom.h"
#include "debug_functions.h"
#include "drdiscord.h"
#include "drpacket.h"
#include "modules/managers/character_manager.h"
#include "modules/managers/localization_manager.h"
#include "drserversocket.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "lobby.h"
#include "version.h"
#include "modules/networking/json_packet.h"

void AOApplication::connect_to_server(DRServerInfo p_server)
{
  m_server_socket->connect_to_server(p_server);
}

void AOApplication::send_server_packet(DRPacket p_packet)
{
  if (!m_server_socket->is_connected())
  {
    qDebug() << "Failed to send packet: not connected to server";
    return;
  }
  qDebug().noquote() << "S/S:" << p_packet.to_string();
  m_server_socket->send_packet(p_packet);
}

AOApplication::ServerStatus AOApplication::last_server_status()
{
  return m_server_status;
}

bool AOApplication::joined_server()
{
  return m_server_status == Joined;
}

void AOApplication::_p_handle_server_state_update(DRServerSocket::ConnectionState p_state)
{
  const ServerStatus l_previous_status = m_server_status;
  switch (p_state)
  {
  case DRServerSocket::NotConnected:
    switch (l_previous_status)
    {
    case Connecting:
      m_server_status = TimedOut;
      break;

    case Joined:
      m_server_status = Disconnected;
      if (is_courtroom_constructed)
      {
        m_courtroom->stop_all_audio();
        call_notice(LocalizationManager::get().getLocalizationText("NOTICE_DISCONNECT"));
        construct_lobby();
        destruct_courtroom();
      }
      break;

    default:
      m_server_status = NotConnected;
      break;
    }
    break;

  case DRServerSocket::Connecting:
    m_server_status = Connecting;
    break;

  case DRServerSocket::Connected:
    m_server_status = Connected;
    break;
  }

  if (m_server_status != l_previous_status)
  {
    emit server_status_changed(m_server_status);
  }
}

void AOApplication::_p_handle_server_packet(DRPacket p_packet)
{
  const QString l_header = p_packet.get_header();
  const QStringList l_content = p_packet.get_content();

  if (l_header != "checkconnection")
    qDebug().noquote() << "S/R:" << p_packet.to_string();

  if (l_header == "decryptor")
  {
    // This packet is maintained as is for legacy purposes,
    // even though its sole argument is no longer used for anything
    // productive
    if (l_content.size() == 0)
      return;

    m_server_client_version = VersionNumber();
    m_server_client_version_status = VersionStatus::NotCompatible;
    send_server_packet(DRPacket("HI", {get_hdid()}));
  }
  else if (l_header == "ID")
  {
    if (l_content.size() < 2)
      return;

    m_client_id = l_content.at(0).toInt();
    m_server_software = l_content.at(1);

    send_server_packet(DRPacket("ID", {"DRO", get_version_string()}));
  }
  else if (l_header == "CT")
  {
    if (l_content.size() < 2)
      return;

    if (is_courtroom_constructed)
      m_courtroom->append_server_chatmessage(l_content.at(0), l_content.at(1));
  }
  else if (l_header == "client_version")
  {
    if (l_content.size() < 3)
      return;

    m_server_client_version = VersionNumber(l_content.at(0).toInt(), l_content.at(1).toInt(), l_content.at(2).toInt());
    const VersionNumber l_client_version = get_version_number();
    if (l_client_version == m_server_client_version)
    {
      m_server_client_version_status = VersionStatus::Ok;
    }
    else if (l_client_version < m_server_client_version)
    {
      m_server_client_version_status = VersionStatus::ClientOutdated;
    }
    else if (l_client_version > m_server_client_version)
    {
      m_server_client_version_status = VersionStatus::ServerOutdated;
    }
  }
  else if (l_header == "PN")
  {
    if (l_content.size() < 2)
      return;

    m_lobby->set_player_count(l_content.at(0).toInt(), l_content.at(1).toInt());
  }
  else if (l_header == "SI")
  {
    if (l_content.size() != 3)
      return;

    m_character_count = l_content.at(0).toInt();
    m_evidence_count = l_content.at(1).toInt();
    m_music_count = l_content.at(2).toInt();

    if (m_character_count < 1 || m_evidence_count < 0 || m_music_count < 0)
      return;

    m_loaded_characters = 0;
    m_loaded_evidence = 0;
    m_loaded_music = 0;
    m_loaded_music_list = false;
    m_loaded_area_list = false;

    construct_courtroom();

    DRServerInfo l_current_server = m_lobby->get_selected_server();
    QString l_window_title = "Danganronpa Online (" + get_version_string() + ")";
    if (!l_current_server.name.isEmpty())
      l_window_title = l_window_title + ": " + l_current_server.to_info();
    m_courtroom->set_window_title(l_window_title);

    m_lobby->show_loading_overlay();
    m_lobby->set_loading_text("Loading");
    m_lobby->set_loading_value(0);

    send_server_packet(DRPacket("RC"));

    dr_discord->set_state(DRDiscord::State::Connected);
    dr_discord->set_server_name(l_current_server.to_info());
  }
  else if (l_header == "CharsCheck")
  {
    if (!is_courtroom_constructed)
      return;

    QVector<char_type> l_chr_list = CharacterManager::get().GetServerCharList();
    if (l_content.length() != l_chr_list.length())
    {
      qWarning() << "Server sent a character list of length " << l_content.length() << "which is different from the expected length " << l_chr_list.length() << "so ignoring it.";
      return;
    }

    for (int i = 0; i < l_chr_list.length(); ++i)
    {
      l_chr_list[i].taken = l_content.at(i) == "-1";
      CharacterManager::get().SetCharaTaken(i, l_content.at(i) == "-1");
    }


    CharacterManager::get().SetCharList(l_chr_list);
  }
  else if (l_header == "SC")
  {
    if (!is_courtroom_constructed)
      return;

    QVector<char_type> l_chr_list;
    for (const QString &i_chr_name : qAsConst(l_content))
    {
      char_type l_chr;
      l_chr.name = i_chr_name;
      l_chr_list.append(std::move(l_chr));
    }
    CharacterManager::get().SetCharList(l_chr_list);
    m_loaded_characters = m_character_count;

    if (is_lobby_constructed)
    {
      m_lobby->set_loading_text("Loading chars:\n" + QString::number(m_loaded_characters) + "/" + QString::number(m_character_count));
      int total_loading_size = m_character_count + m_evidence_count + m_music_count;
      int loading_value = (m_loaded_characters / static_cast<double>(total_loading_size)) * 100;
      m_lobby->set_loading_value(loading_value);

      send_server_packet(DRPacket("RM"));
    }
  }
  else if (l_header == "SM") // TODO remove block for 1.2.0+
  {
    if (!is_courtroom_constructed)
      return;

    QStringList l_area_list;
    QStringList l_music_list;

    for (int i = 0; i < l_content.length(); ++i)
    {
      bool l_found_music = false;

      { // look for first song
        const QString &i_value = l_content.at(i);
        for (const QString &i_ext : audio_extensions(true))
        {
          if (!i_value.endsWith(i_ext, Qt::CaseInsensitive))
            continue;
          l_found_music = true;
          break;
        }
      }

      if (!l_found_music)
        continue;
      l_area_list = l_content.mid(0, i - 1);
      l_music_list = l_content.mid(i - 1);
      break;
    }
    m_courtroom->set_area_list(l_area_list);
    m_courtroom->set_music_list(l_music_list);

    m_loaded_music = m_music_count;
    m_lobby->set_loading_text("Loading music:\n" + QString::number(m_loaded_music) + "/" + QString::number(m_music_count));
    int total_loading_size = m_character_count + m_evidence_count + m_music_count;
    int loading_value = ((m_loaded_characters + m_loaded_evidence + m_loaded_music) / static_cast<double>(total_loading_size)) * 100;
    m_lobby->set_loading_value(loading_value);
    send_server_packet(DRPacket("RD"));
  }
  else if (l_header == "JSN")
  {
    JsonPacket::ProcessJson(l_content.at(0));
  }
  else if (l_header == "LIST_REASON")
  {
    int prompt = l_content.at(0).toInt();
    m_courtroom->m_current_reportcard_reason = Courtroom::ReportCardReason(prompt);

    m_courtroom->m_area_description = l_content.at(1);

    m_courtroom->write_area_desc();
    AOApplication::getInstance()->m_courtroom->construct_playerlist_layout();
  }
  else if (l_header == "FA")
  {
    if (!is_courtroom_constructed)
      return;
    m_courtroom->set_area_list(l_content);

    if (!m_loaded_area_list && is_lobby_constructed)
    {
      m_lobby->set_loading_text("Loading areas...");
    }
    m_loaded_area_list = true;
  }
  else if (l_header == "FM")
  {
    if (!is_courtroom_constructed)
      return;
    m_courtroom->set_music_list(l_content);

    if (!m_loaded_area_list && is_lobby_constructed)
    {
      m_lobby->set_loading_text("Loading music...");
      send_server_packet(DRPacket("RD"));
    }
    m_loaded_music_list = true;
  }
  else if (l_header == "DONE")
  {
    if (!is_courtroom_constructed)
      return;

    m_courtroom->done_received();
    m_server_status = Joined;
    emit server_status_changed(m_server_status);

    destruct_lobby();
  }
  else if (l_header == "joined_area")
  {
    if (!is_courtroom_constructed)
      return;

    m_courtroom->reset_viewport();
  }
  else if (l_header == "BN")
  {
    if (l_content.size() < 1)
      return;

    if (!is_courtroom_constructed)
      return;

    DRAreaBackground l_area_bg;
    l_area_bg.background = l_content.at(0);

    for (int i = 1; i < l_content.size(); ++i)
    {
      const QStringList l_tod_data = l_content.at(i).split("|", DR::SkipEmptyParts);
      if (l_tod_data.size() < 2)
        continue;
      l_area_bg.background_tod_map.insert(l_tod_data.at(0), l_tod_data.at(1));
    }

    qDebug() << l_area_bg.background << l_area_bg.background_tod_map;

    m_courtroom->set_background(l_area_bg);
  }
  else if (l_header == "area_ambient")
  {
    if (l_content.size() < 1)
      return;

    if (!is_courtroom_constructed)
      return;

    m_courtroom->set_ambient(l_content.at(0));
  }
  else if (l_header == "chat_tick_rate")
  {
    if (l_content.size() < 1)
      return;
    if (!is_courtroom_constructed)
      return;
    m_courtroom->set_tick_rate(l_content.at(0).toInt());
  }
  // server accepting char request(CC) packet
  else if (l_header == "PV")
  {
    if (l_content.size() < 3)
      return;

    if (is_courtroom_constructed)
      m_courtroom->set_character_id(l_content.at(2).toInt());
  }
  else if (l_header == "MS")
  {
    if (is_courtroom_constructed && joined_server())
      m_courtroom->next_chatmessage(l_content);
  }
  else if (l_header == "ackMS")
  {
    if (is_courtroom_constructed && joined_server())
      m_courtroom->handle_acknowledged_ms();
  }
  else if (l_header == "MC")
  {
    if (is_courtroom_constructed && joined_server())
      m_courtroom->handle_song(l_content);
  }
  else if (l_header == "RT")
  {
    if (l_content.size() < 1)
      return;
    if (is_courtroom_constructed)
      m_courtroom->handle_wtce(l_content.at(0));
  }
  else if (l_header == "HP")
  {
    if (is_courtroom_constructed && l_content.size() > 1)
      m_courtroom->set_hp_bar(l_content.at(0).toInt(), l_content.at(1).toInt());
  }
  else if (l_header == "KK")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
    {
      int f_cid = m_courtroom->get_character_id();
      int remote_cid = l_content.at(0).toInt();

      if (f_cid != remote_cid && remote_cid != -1)
        return;

      call_notice(LocalizationManager::get().getLocalizationText("NOTICE_KICKED"));
      leave_server();
      construct_lobby();
      destruct_courtroom();
    }
  }
  else if (l_header == "KB")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->set_ban(l_content.at(0).toInt());
  }
  else if (l_header == "BD")
  {
    call_notice(LocalizationManager::get().getLocalizationText("NOTICE_BANNED_2"));
  }
  else if (l_header == "ZZ")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->mod_called(l_content.at(0));
  }
  else if (l_header == "CL")
  {
    if (is_courtroom_constructed && l_content.size() > 0)
      m_courtroom->handle_clock(l_content.at(1));
  }
  else if (l_header == "GM")
  {
    if (l_content.length() < 1)
      return;
    ao_config->set_gamemode(l_content.at(0));
  }
  else if (l_header == "TOD")
  {
    if (l_content.length() < 1)
      return;
    ao_config->set_timeofday(l_content.at(0));
  }
  else if (l_header == "TR")
  {
    // Timer resume
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    m_courtroom->resume_timer(timer_id);
  }
  else if (l_header == "TST")
  {
    // Timer set time
    if (l_content.size() != 2)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    int new_time = l_content.at(1).toInt();
    m_courtroom->set_timer_time(timer_id, new_time);
  }
  else if (l_header == "TSS")
  {
    // Timer set timeStep length
    if (l_content.size() != 2)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    int timestep_length = l_content.at(1).toInt();
    m_courtroom->set_timer_timestep(timer_id, timestep_length);
  }
  else if (l_header == "TSF")
  {
    // Timer set Firing interval
    if (l_content.size() != 2)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    int firing_interval = l_content.at(1).toInt();
    m_courtroom->set_timer_firing(timer_id, firing_interval);
  }
  else if (l_header == "TP")
  {
    // Timer pause
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    int timer_id = l_content.at(0).toInt();
    m_courtroom->pause_timer(timer_id);
  }
  else if (l_header == "SP")
  {
    // Set position
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    m_courtroom->set_character_position(l_content.at(0));
  }
  else if (l_header == "SN")
  {
    if (l_content.size() != 1)
      return;
    if (!is_courtroom_constructed)
      return;
    const QString &l_showname = l_content.at(0);
    if (ao_config->showname() != l_showname)
      m_courtroom->ignore_next_showname();
    ao_config->set_showname(l_showname);
  }
}
