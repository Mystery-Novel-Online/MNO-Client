#include "legacy_network_handler.h"

#include <modules/networking/packets/packet_play_music.h>
#include <modules/networking/packets/packets_metadata.h>
#include <modules/networking/packets/packets_scene.h>

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


}
