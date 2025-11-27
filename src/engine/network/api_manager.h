#ifndef API_MANAGER_H
#define API_MANAGER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class ApiManager : public QObject
{
  Q_OBJECT

public:
  static ApiManager& instance();

  QNetworkReply* get(const QString& url);
  QNetworkReply* post(const QString& url, const QByteArray& data);
  QNetworkReply* post(const QString& url, QHttpMultiPart* multiPart);

  static QString repoUrl(int characterId);
  static QString baseUri();
  static QString authorizationKey();

private:
  explicit ApiManager(QObject *parent = nullptr);
  ApiManager(const ApiManager&) = delete;
  ApiManager& operator=(const ApiManager&) = delete;

  QNetworkAccessManager m_network;
};

#endif // API_MANAGER_H
