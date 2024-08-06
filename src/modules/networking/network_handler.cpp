#include "network_handler.h"

NetworkHandler::NetworkHandler()
{
}

bool NetworkHandler::ProcessPacket(QString t_operation, QStringList t_contents)
{
  if(!m_PacketMap.contains(t_operation)) return false;
  m_PacketMap[t_operation]->HandleIncoming(t_contents);
  return true;
}

void NetworkHandler::RegisterPacket(QString t_operation, DROPacket *t_packet)
{
  m_PacketMap[t_operation] = t_packet;
}
