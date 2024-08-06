#ifndef PACKET_PLAY_MUSIC_H
#define PACKET_PLAY_MUSIC_H

#include <modules/networking/dro_packet.h>


class PacketPlayMusic : public DROPacket
{
public:
  PacketPlayMusic();

public:
  void HandleIncoming(QStringList t_Contents);
};

#endif // PACKET_PLAY_MUSIC_H
