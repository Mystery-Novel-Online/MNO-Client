#ifndef NETWORKTRAFFIC_H
#define NETWORKTRAFFIC_H

enum class NetworkDirection
{
  Incoming,
  Outgoing
};

struct NetworkEvent
{
  QDateTime timestamp;
  NetworkDirection direction;

  QString header;
  QStringList content;

  QString raw;
};

class NetworkTrafficLog
{
public:
  static NetworkTrafficLog& instance();
  void add(const NetworkEvent& event);
  void clear();
  QVector<NetworkEvent> events() const { return m_events; };

private:
  mutable QMutex m_mutex;
  QVector<NetworkEvent> m_events;
};

#endif // NETWORKTRAFFIC_H
