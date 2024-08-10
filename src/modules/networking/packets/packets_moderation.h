#ifndef PACKETS_MODERATION_H
#define PACKETS_MODERATION_H

#include <modules/networking/dro_packet.h>


class PacketModCalled : public DROPacket
{
public:
  PacketModCalled(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketKicked : public DROPacket
{
public:
  PacketKicked(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketKickBanned : public DROPacket
{
public:
  PacketKickBanned(){};
  void HandleIncoming(QStringList t_Contents);
};
#endif // PACKETS_MODERATION_H
