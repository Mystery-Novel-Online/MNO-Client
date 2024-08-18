#ifndef LEGACY_NETWORK_HANDLER_H
#define LEGACY_NETWORK_HANDLER_H

#include "network_handler.h"

class LegacyNetworkHandler : public NetworkHandler
{
public:
  LegacyNetworkHandler();

public:
  void GeneratePacketMap();
  void SendPlayMusic(QString t_Name, int t_CharId);

public:
  void ProcessMetalistAreas(QHash<int, QString> t_AreaList);
  void ProcessReachableAreas(QList<int> t_ReachableAreas);
};

#endif // LEGACY_NETWORK_HANDLER_H
