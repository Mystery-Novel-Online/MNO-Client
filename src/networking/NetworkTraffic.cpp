#include "NetworkTraffic.h"

NetworkTrafficLog &NetworkTrafficLog::instance()
{
  static NetworkTrafficLog instance;
  return instance;
}

void NetworkTrafficLog::add(const NetworkEvent &event)
{
  m_events.append(event);
}

void NetworkTrafficLog::clear()
{
  m_events.clear();
}
