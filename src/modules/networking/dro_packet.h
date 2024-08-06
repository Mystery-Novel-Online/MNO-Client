#ifndef DRO_PACKET_H
#define DRO_PACKET_H
#include <QStringList>

class DROPacket
{
public:
  DROPacket();
  virtual void HandleIncoming(QStringList t_Contents) = 0;
};

#endif // DRO_PACKET_H
