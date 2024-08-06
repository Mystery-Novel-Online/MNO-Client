#ifndef NEO_NETWORK_HANDLER_H
#define NEO_NETWORK_HANDLER_H

#include "network_handler.h"

class NeoNetworkHandler : public NetworkHandler
{
public:
  NeoNetworkHandler();

public:
  void GeneratePacketMap();
};

#endif // NEO_NETWORK_HANDLER_H
