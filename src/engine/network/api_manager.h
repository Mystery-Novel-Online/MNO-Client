#ifndef API_MANAGER_H
#define API_MANAGER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

enum ApiPermissionLevels
{
  APIPerms_None = 0,
  APIPerms_User = 1,
  APIPerms_Auto = 2,
  APIPerms_Mod = 3
};
class ApiManager : public QObject
{
  Q_OBJECT

public:
  static ApiManager& instance();

  QNetworkReply* get(const QString& url);
  QNetworkReply* post(const QString& url, const QByteArray& data);
  QNetworkReply* post(const QString& url, QHttpMultiPart* multiPart);

  void login();

  static QString repoUrl(int characterId);
  static QString baseUri();
  static QString authorizationKey();
  ApiPermissionLevels userPermission() { return m_permissionLevel;};

  static bool appendFile(QHttpMultiPart* multipart, const QString& fieldName, const QString& filePath);
  static void appendField(QHttpMultiPart* multipart, const QString& name, const QString& value);

signals:
  void loginStatus(bool state, std::string key);

public slots:

private:
  explicit ApiManager(QObject *parent = nullptr);
  ApiManager(const ApiManager&) = delete;
  ApiManager& operator=(const ApiManager&) = delete;

  QNetworkAccessManager m_network;
  ApiPermissionLevels m_permissionLevel = APIPerms_None;
};

#endif // API_MANAGER_H
