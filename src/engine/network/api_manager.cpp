#include "api_manager.h"
#include <QNetworkRequest>

ApiManager::ApiManager(QObject *parent)
    : QObject(parent)
{
}

ApiManager& ApiManager::instance()
{
  static ApiManager inst;
  return inst;
}

QNetworkReply* ApiManager::get(const QString &url)
{
  QNetworkRequest request(url);
  return m_network.get(request);
}

QNetworkReply* ApiManager::post(const QString &url, const QByteArray &data)
{
  QNetworkRequest request(url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  return m_network.post(request, data);
}
