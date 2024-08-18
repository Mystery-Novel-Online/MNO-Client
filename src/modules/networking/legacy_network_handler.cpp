#include "legacy_network_handler.h"

#include <modules/networking/packets/packet_play_music.h>
#include <modules/networking/packets/packets_metadata.h>
#include <modules/networking/packets/packets_moderation.h>
#include <modules/networking/packets/packets_scene.h>
#include <modules/networking/packets/packets_timer.h>

#include <aoapplication.h>


LegacyNetworkHandler::LegacyNetworkHandler()
{
}

void LegacyNetworkHandler::GeneratePacketMap()
{
  RegisterPacket("MC", new PacketPlayMusic());
  RegisterPacket("SCENE", new PacketScene());
  RegisterPacket("joined_area", new PacketJoinedArea(), true);
  RegisterPacket("area_ambient", new PacketAreaAmbience(), true, 1);
  RegisterPacket("WEA", new PacketAreaWeather(), true, 1);
  RegisterPacket("BN", new PacketAreaBackground(), true, 1);
  RegisterPacket("CharsCheck", new PacketCharaCheck(), true);
  RegisterPacket("HP", new PacketAreaHP(), true, 1);

  RegisterPacket("FM", new PacketMusicList(), true);
  RegisterPacket("FA", new PacketAreaList(), true);

  RegisterPacket("TR", new PacketTimerResume(), true, 1);
  RegisterPacket("TST", new PacketTimerSetTime(), true, 2);
  RegisterPacket("TSS", new PacketTimerLength(), true, 2);
  RegisterPacket("TSF", new PacketTimerFiringInterval(), true, 2);
  RegisterPacket("TP", new PacketTimerPause(), true, 1);

  RegisterPacket("CL", new PacketClock(), true, 2);
  RegisterPacket("GM", new PacketGameMode(), true, 1);
  RegisterPacket("TOD", new PacketTimeOfDay(), true, 1);
  RegisterPacket("ANI", new PacketPlayAnimation(), true, 1);

  RegisterPacket("chat_tick_rate", new PacketChatTickRate(), true, 1);
  RegisterPacket("MS", new PacketICMessage(), true);
  RegisterPacket("CT", new PacketOOCMessage(), true, 2);

  RegisterPacket("SP", new PacketSetBGPosition(), true, 1);
  RegisterPacket("SN", new PacketSetShowname(), true, 1);
  RegisterPacket("RT", new PacketPlayWTCE(), true, 1);

  RegisterPacket("ZZ", new PacketModCalled(), true, 1);
  RegisterPacket("KB", new PacketKickBanned(), true, 1);
  RegisterPacket("KK", new PacketKicked(), true, 1);
  RegisterPacket("SC", new PacketCharaList(), true);
  RegisterPacket("INVES", new PacketInvestigation(), true, 1);



}

void LegacyNetworkHandler::SendPlayMusic(QString t_Name, int t_CharId)
{
  AOApplication::getInstance()->send_server_packet(DRPacket("MC", {t_Name, QString::number(t_CharId)}));
}

void LegacyNetworkHandler::ProcessMetalistAreas(QHash<int, QString> t_AreaList)
{

}

void LegacyNetworkHandler::ProcessReachableAreas(QList<int> t_ReachableAreas)
{

}
