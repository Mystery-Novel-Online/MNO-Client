#ifndef WORKSHOP_CACHE_H
#define WORKSHOP_CACHE_H

class WorkshopCache : public QObject
{
  Q_OBJECT
public:
  WorkshopCache(const QString &cacheDir, const QString &dbFile, QObject *parent = nullptr);
  void downloadFile(const QUrl& url);

  QString getUrlForHash(const QString &hash) const;
  QString getHashForUrl(const QUrl &url) const {return m_urlToHash.value(url.toString());};

private:
  void loadDatabase();
  void saveDatabase();
  void updateAccessTime(const QString &hash);

  void cleanup();

signals:
  void fileCached(const QString &filePath, const QString &hash);

private:
  QString m_cacheDirectory;

  QString m_dbFile;
  QNetworkAccessManager m_netManager;
  QJsonObject m_db;
  QHash<QString, QString> m_urlToHash;
};

#endif // WORKSHOP_CACHE_H
