#ifndef PACKETS_METADATA_H
#define PACKETS_METADATA_H

#include <modules/networking/dro_packet.h>


class PacketMusicList : public DROPacket
{
public:
  PacketMusicList(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketAreaList : public DROPacket
{
public:
  PacketAreaList(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketCharaList : public DROPacket
{
public:
  PacketCharaList(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketCharaCheck : public DROPacket
{
public:
  PacketCharaCheck(){};
  void HandleIncoming(QStringList t_Contents);
};

class NeoPacketAreaList : public DROPacket
{
public:
  NeoPacketAreaList(){};
  void HandleIncoming(QStringList t_Contents);
};

class NeoPacketReachableList : public DROPacket
{
public:
  NeoPacketReachableList(){};
  void HandleIncoming(QStringList t_Contents);
};



#endif // PACKETS_METADATA_H
