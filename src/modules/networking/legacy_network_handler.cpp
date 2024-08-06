#include "legacy_network_handler.h"

#include <modules/networking/packets/packet_play_music.h>
#include <modules/networking/packets/packets_scene.h>

LegacyNetworkHandler::LegacyNetworkHandler()
{
}

void LegacyNetworkHandler::GeneratePacketMap()
{
  RegisterPacket("MC", new PacketPlayMusic());
  RegisterPacket("SCENE", new PacketScene());
}
