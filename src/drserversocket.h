#pragma once


#include "drpacket.h"

#include <QAbstractSocket>
#include <QObject>

class QTcpSocket;
class QTimer;

class DRServerSocket : public QObject
{
  Q_OBJECT

public:
  enum ConnectionState
  {
    NotConnected,
    Connecting,
    Connected,
  };

  DRServerSocket(QObject *parent = nullptr);

  bool is_connected() const;

public slots:
  void connect_to_server(DRServerInfo server);

  void disconnect_from_server();

  void send_packet(DRPacket packet);

signals:
  void connection_state_changed(ConnectionState);
  void packet_received(DRPacket);
  void socket_error(QString);

private:
  static const int CONNECTING_DELAY;

  DRServerInfo m_server;
  QTcpSocket *m_socket = nullptr;
  QTimer *m_connecting_timeout = nullptr;
  ConnectionState m_state = NotConnected;
  QString m_buffer;

private slots:
  void _p_update_state(QAbstractSocket::SocketState);
  void _p_check_socket_error();
  void _p_read_socket();
};
