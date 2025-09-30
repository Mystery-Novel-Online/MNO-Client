#include "workshop_cache.h"

WorkshopCache::WorkshopCache(const QString &cacheDir, const QString &dbFile, QObject *parent) : QObject(parent), m_cacheDirectory(cacheDir), m_dbFile(dbFile)
{
  if (!QDir(m_cacheDirectory).exists())
    QDir().mkpath(m_cacheDirectory);
  loadDatabase();
}

void WorkshopCache::downloadFile(const QUrl& url)
{
  QString qUrlString = url.toString();
  if (m_urlToHash.contains(qUrlString))
  {
    QString qHashCache = m_urlToHash.value(qUrlString);
    QString qFileCache = QDir(m_cacheDirectory).filePath(qHashCache);
    if (QFile::exists(qFileCache))
    {
      updateAccessTime(qHashCache);
      emit fileCached(qFileCache, qHashCache);
      return;
    }
    else
    {
      m_db.remove(qHashCache);
      m_urlToHash.remove(qUrlString);
      saveDatabase();
    }
  }

  QNetworkReply* reply = m_netManager.get(QNetworkRequest(url));
  connect(reply, &QNetworkReply::finished, this, [this, reply, qUrlString]()
  {
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError) {
      qWarning() << "Download failed:" << reply->errorString();
      return;
    }

    QByteArray data = reply->readAll();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    QString hashHex = hash.toHex();
    QString filePath = QDir(m_cacheDirectory).filePath(hashHex);

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly))
    {
      qWarning() << "Failed to write file:" << filePath;
      return;
    }
    file.write(data);
    file.close();

    QJsonObject entry;
    entry["url"] = qUrlString;
    entry["lastAccess"] = QDateTime::currentSecsSinceEpoch();
    m_db[hashHex] = entry;
    m_urlToHash[qUrlString] = hashHex;
    saveDatabase();

    emit fileCached(filePath, hashHex);
  });
}

void WorkshopCache::loadDatabase()
{
  QFile file(m_dbFile);
  if (!file.exists())
    return;

  if (file.open(QIODevice::ReadOnly))
  {
    QByteArray content = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(content);
    if (doc.isObject()) {
      m_db = doc.object();
      for (auto it = m_db.constBegin(); it != m_db.constEnd(); ++it) {
        auto entry = it.value().toObject();
        m_urlToHash[entry.value("url").toString()] = it.key();
      }
    }
  }
}

void WorkshopCache::saveDatabase()
{
  QFile file(m_dbFile);
  if (!file.open(QIODevice::WriteOnly)) {
    qWarning() << "Failed to write database file:" << m_dbFile;
    return;
  }
  QJsonDocument doc(m_db);
  file.write(doc.toJson());
}

void WorkshopCache::updateAccessTime(const QString &hash)
{
  if (m_db.contains(hash)) {
    auto entry = m_db.value(hash).toObject();
    entry["lastAccess"] = QDateTime::currentSecsSinceEpoch();
    m_db[hash] = entry;
    saveDatabase();
  }
}

void WorkshopCache::cleanup()
{

}

QString WorkshopCache::getUrlForHash(const QString &hash) const
{
  auto entry = m_db.value(hash).toObject();
  return entry.value("url").toString();
}
