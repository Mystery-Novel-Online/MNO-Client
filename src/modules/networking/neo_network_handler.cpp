#include "neo_network_handler.h"

#include <modules/networking/packets/packet_play_music.h>

NeoNetworkHandler::NeoNetworkHandler()
{
}

void NeoNetworkHandler::GeneratePacketMap()
{
  RegisterPacket("MC", new PacketPlayMusic());
}
