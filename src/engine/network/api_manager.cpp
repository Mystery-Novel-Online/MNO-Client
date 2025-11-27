#include "api_manager.h"
#include <QHttpMultiPart>
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
  QNetworkRequest request(baseUri() + url);
  return m_network.get(request);
}

QNetworkReply* ApiManager::post(const QString &url, const QByteArray &data)
{
  QNetworkRequest request(baseUri() + url);
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  return m_network.post(request, data);
}

QNetworkReply* ApiManager::post(const QString& url, QHttpMultiPart* multiPart)
{
  QNetworkRequest request(baseUri() + url);
  QNetworkReply* reply = m_network.post(request, multiPart);

  multiPart->setParent(reply);

  return reply;
}

QString ApiManager::repoUrl(int characterId)
{
  return baseUri() + "api/workshop/" + QString::number(characterId) + "/repo";
}

QString ApiManager::baseUri()
{
  return QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/") );
}

QString ApiManager::authorizationKey()
{
  return QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE"));
}
