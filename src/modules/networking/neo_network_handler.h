#ifndef NEO_NETWORK_HANDLER_H
#define NEO_NETWORK_HANDLER_H

#include "network_handler.h"

class NeoNetworkHandler : public NetworkHandler
{
public:
  NeoNetworkHandler();

public:
  void GeneratePacketMap();
  void SendPlayMusic(QString t_Name, int t_CharId);

public:
  void ProcessMetalistAreas(QHash<int, QString> t_AreaList);
  void ProcessReachableAreas(QList<int> t_ReachableAreas);

private:
  QHash<int, QString> m_CurrentAreaList = {};
};

#endif // NEO_NETWORK_HANDLER_H
