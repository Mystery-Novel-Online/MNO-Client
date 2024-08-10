#ifndef PACKETS_SCENE_H
#define PACKETS_SCENE_H

#include <modules/networking/dro_packet.h>



class NeoPacketScene : public DROPacket
{
public:
  NeoPacketScene(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketScene : public DROPacket
{
public:
  PacketScene(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketJoinedArea : public DROPacket
{
public:
  PacketJoinedArea(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketAreaAmbience : public DROPacket
{
public:
  PacketAreaAmbience(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketAreaWeather : public DROPacket
{
public:
  PacketAreaWeather(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketAreaBackground : public DROPacket
{
public:
  PacketAreaBackground(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketAreaHP : public DROPacket
{
public:
  PacketAreaHP(){};
  void HandleIncoming(QStringList t_Contents);
};
#endif
