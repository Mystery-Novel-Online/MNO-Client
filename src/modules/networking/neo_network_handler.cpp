#include "neo_network_handler.h"

#include <modules/networking/packets/packet_play_music.h>
#include <modules/networking/packets/packets_scene.h>

NeoNetworkHandler::NeoNetworkHandler()
{
}

void NeoNetworkHandler::GeneratePacketMap()
{
  RegisterPacket("MC", new PacketPlayMusic());
  RegisterPacket("SCENE", new NeoPacketScene());
}
