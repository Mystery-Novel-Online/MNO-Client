#ifndef PACKETS_TIMER_H
#define PACKETS_TIMER_H

#include <modules/networking/dro_packet.h>


class PacketTimerResume : public DROPacket
{
public:
  PacketTimerResume(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketTimerSetTime : public DROPacket
{
public:
  PacketTimerSetTime(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketTimerLength : public DROPacket
{
public:
  PacketTimerLength(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketTimerFiringInterval : public DROPacket
{
public:
  PacketTimerFiringInterval(){};
  void HandleIncoming(QStringList t_Contents);
};

class PacketTimerPause : public DROPacket
{
public:
  PacketTimerPause(){};
  void HandleIncoming(QStringList t_Contents);
};
#endif // PACKETS_TIMER_H
