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

#include <modules/managers/game_manager.h>
#include <modules/managers/variable_manager.h>

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

  if (l_header != "checkconnection") qDebug().noquote() << "S/R:" << p_packet.to_string();

  if(GameManager::get().ProcessIncomingPacket(l_header, l_content))
  {
    return;
  }


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
  else if (l_header == "BD")
  {
    call_notice(LocalizationManager::get().getLocalizationText("NOTICE_BANNED_2"));
  }
  else if (l_header == "FL")
  {
    GameManager::get().setServerFunctions(l_content);
  }
  else if (l_header == "ID")
  {
    if (l_content.size() < 2)
      return;

    m_client_id = l_content.at(0).toInt();
    m_server_software = l_content.at(1);

    send_server_packet(DRPacket("ID", {"DRO", get_version_string()}));
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
  else if (l_header == "DONE")
  {
    if (!is_courtroom_constructed)
      return;

    m_courtroom->done_received();
    m_server_status = Joined;
    emit server_status_changed(m_server_status);

    destruct_lobby();
  }
  //Everything below here should be moved.
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
    m_courtroom->set_music_list(ScenarioManager::get().ParseMusicList(l_music_list));

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
  // server accepting char request(CC) packet
  else if (l_header == "PV")
  {
    if (l_content.size() < 3)
      return;

    if (is_courtroom_constructed)
      m_courtroom->set_character_id(l_content.at(2).toInt());
  }
  else if (l_header == "ackMS")
  {
    if (is_courtroom_constructed && joined_server())
      m_courtroom->handle_acknowledged_ms();
  }
}
