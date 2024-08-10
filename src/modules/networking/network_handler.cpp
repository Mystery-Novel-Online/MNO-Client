#include "network_handler.h"

#include <aoapplication.h>

NetworkHandler::NetworkHandler()
{
}

bool NetworkHandler::ProcessPacket(QString t_operation, QStringList t_contents)
{
  if(!m_PacketMap.contains(t_operation)) return false;

  if (t_contents.size() < m_ParamCount[t_operation]) return false;

  if(m_RequireCourtroom[t_operation])
  {
    if (!AOApplication::getInstance()->GetCourtroomConstructed()) return false;
  }

  m_PacketMap[t_operation]->HandleIncoming(t_contents);
  return true;
}

void NetworkHandler::RegisterPacket(QString t_operation, DROPacket *t_packet, bool l_RequireCourtroom, int l_MinimumParams)
{
  m_PacketMap[t_operation] = t_packet;
  m_RequireCourtroom[t_operation] = l_RequireCourtroom;
  m_ParamCount[t_operation] = l_MinimumParams;
}
