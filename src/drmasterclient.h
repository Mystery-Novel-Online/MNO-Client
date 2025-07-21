#pragma once

// std
#include <functional>

// qt
#include <QObject>
#include <QVariant>
#include <QVector>

class QNetworkAccessManager;

// src


class DRMasterClient : public QObject
{
  Q_OBJECT

public:
  DRMasterClient(QObject *parent = nullptr);
  ~DRMasterClient();

  QString address() const;
  QString motd() const;
  DRServerInfoList server_list() const;

public slots:
  void set_address(QString address);
  void request_motd();
  void request_server_list();

signals:
  void address_changed();
  void motd_changed();
  void server_list_changed();

private:
  QNetworkAccessManager *m_network = nullptr;
  QString m_address;
  QString m_motd;
  DRServerInfoList m_server_list;
  using Delegate = void (DRMasterClient::*)(QVariant);
  QHash<QObject *, Delegate> m_pending_delegates;

private slots:
  void send_get_request(QString request, Delegate delegate);
  void process_request();
  void process_motd(QVariant);
  void process_server_list(QVariant);
};
