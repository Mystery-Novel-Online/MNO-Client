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
};

#endif // LEGACY_NETWORK_HANDLER_H
