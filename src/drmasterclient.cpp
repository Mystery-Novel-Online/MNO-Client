#include "drmasterclient.h"
#include "pch.h"

DRMasterClient::DRMasterClient(QObject *parent)
    : QObject(parent)
    , m_network(new QNetworkAccessManager(this))
{}

DRMasterClient::~DRMasterClient()
{}

QString DRMasterClient::address() const
{
  return m_address;
}

void DRMasterClient::set_address(QString p_address)
{
  m_address = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/"));
  emit address_changed();
}

QString DRMasterClient::motd() const
{
  return m_motd;
}

void DRMasterClient::request_motd()
{
  send_get_request("/api/servers/motd", &DRMasterClient::process_motd);
}

DRServerInfoList DRMasterClient::server_list() const
{
  return m_server_list;
}

void DRMasterClient::request_server_list()
{
  send_get_request("/api/servers/browse", &DRMasterClient::process_server_list);
}

void DRMasterClient::send_get_request(QString request, Delegate delegate)
{
  if (m_address.isEmpty())
  {
    qWarning() << "error: address is undefined";
    return;
  }

  QNetworkRequest l_request(m_address + request);
  QNetworkReply *l_reply = m_network->get(l_request);
  l_reply->setParent(this);
  m_pending_delegates.insert(l_reply, delegate);
  connect(l_reply, SIGNAL(finished()), this, SLOT(process_request()));
}

void DRMasterClient::process_request()
{
  QNetworkReply *l_reply = dynamic_cast<QNetworkReply *>(sender());
  if (!l_reply)
  {
    qCritical() << "error: sender is not expected object" << sender();
    return;
  }

  QVariant l_data = l_reply->readAll();
  const QJsonDocument l_doc = QJsonDocument::fromJson(l_data.toByteArray());
  if (!l_doc.isNull())
    l_data = l_doc.toVariant();

  const Delegate l_delegate = m_pending_delegates.take(l_reply);
  (this->*l_delegate)(l_data);
  l_reply->deleteLater();
}

void DRMasterClient::process_motd(QVariant p_data)
{
  m_motd = p_data.toString();
  emit motd_changed();
}

void DRMasterClient::process_server_list(QVariant p_data)
{
  DRServerInfoList l_server_list;
  const QJsonArray l_json_array = p_data.toJsonArray();
  for (const QJsonValue &i_value : l_json_array)
  {
    const QJsonObject i_object = i_value.toObject();
    DRServerInfo l_server;
    l_server.name = i_object.value("name").toString();
    l_server.description = i_object.value("description").toString();
    l_server.address = i_object.value("ip").toString();
    l_server.port = i_object.value("port").toInt();
    l_server_list.append(std::move(l_server));
  }
  m_server_list = std::move(l_server_list);
  emit server_list_changed();
}
