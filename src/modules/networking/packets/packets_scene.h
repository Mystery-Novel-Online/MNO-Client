#ifndef PACKETS_SCENE_H
#define PACKETS_SCENE_H

#include <modules/networking/dro_packet.h>



class NeoPacketScene : public DROPacket
{
public:
  NeoPacketScene(){};

public:
  void HandleIncoming(QStringList t_Contents);
};


class PacketScene : public DROPacket
{
public:
  PacketScene(){};

public:
  void HandleIncoming(QStringList t_Contents);
};

#endif // PACKETS_SCENE_H
