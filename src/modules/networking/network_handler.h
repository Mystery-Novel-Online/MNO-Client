#ifndef NETWORK_HANDLER_H
#define NETWORK_HANDLER_H

#include "dro_packet.h"

#include <QString>
#include <QHash>

class NetworkHandler
{
public:
  NetworkHandler();

  virtual void GeneratePacketMap() = 0;
  bool ProcessPacket(QString t_operation, QStringList t_contents);

  void RegisterPacket(QString t_operation, DROPacket * t_packet);

private:
  QHash<QString, DROPacket *> m_PacketMap = {};
};

#endif // NETWORK_HANDLER_H
