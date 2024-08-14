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

class PacketClock : public DROPacket
{
public:
  PacketClock(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketTimeOfDay : public DROPacket
{
public:
  PacketTimeOfDay(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketGameMode : public DROPacket
{
public:
  PacketGameMode(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketPlayAnimation : public DROPacket
{
public:
  PacketPlayAnimation(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketChatTickRate : public DROPacket
{
public:
  PacketChatTickRate(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketICMessage : public DROPacket
{
public:
  PacketICMessage(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketOOCMessage : public DROPacket
{
public:
  PacketOOCMessage(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketSetBGPosition : public DROPacket
{
public:
  PacketSetBGPosition(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketSetShowname : public DROPacket
{
public:
  PacketSetShowname(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketPlayWTCE : public DROPacket
{
public:
  PacketPlayWTCE(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketCamera : public DROPacket
{
public:
  PacketCamera(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketInvestigation : public DROPacket
{
public:
  PacketInvestigation(){};
  void HandleIncoming(QStringList t_Contents);
};
#endif
