#include "legacy_network_handler.h"

#include <modules/networking/packets/packet_play_music.h>

LegacyNetworkHandler::LegacyNetworkHandler()
{
}

void LegacyNetworkHandler::GeneratePacketMap()
{
  RegisterPacket("MC", new PacketPlayMusic());
}
