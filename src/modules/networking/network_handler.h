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
  void RegisterPacket(QString t_operation, DROPacket * t_packet, bool l_RequireCourtroom = false, int l_MinimumParams = 0);

public:
  virtual void ProcessMetalistAreas(QHash<int, QString> t_AreaList) = 0;
  virtual void ProcessReachableAreas(QList<int> t_ReachableAreas) = 0;


public:
  virtual void SendPlayMusic(QString t_Name, int t_CharId) = 0;

private:
  QHash<QString, DROPacket *> m_PacketMap = {};
  QHash<QString, int> m_ParamCount = {};
  QHash<QString, bool> m_RequireCourtroom = {};
};

#endif // NETWORK_HANDLER_H
