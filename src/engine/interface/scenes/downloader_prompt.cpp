#include "downloader_prompt.h"
#include "engine/fs/fs_characters.h"
#include "engine/fs/fs_mounting.h"
#include "engine/system/user_database.h"

#include <engine/network/api_manager.h>


QMap<QString, QString> buildFileHashMap(const QString &scanDirectory)
{
  QMap<QString, QString> fileHashMap;

  QDirIterator it(scanDirectory,
                  QDir::Files,
                  QDirIterator::Subdirectories);

  while (it.hasNext())
  {
    QString filePath = it.next();

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
      continue;

    QByteArray data = file.readAll();
    QByteArray hash = QCryptographicHash::hash(data, QCryptographicHash::Md5);

    fileHashMap.insert(filePath, QString(hash.toHex()));
  }

  return fileHashMap;
}

DownloaderPrompt::DownloaderPrompt(QWidget *parent) : QDialog{parent}
{
  setWindowTitle("Downloading...");
  setModal(true);
  setFixedSize(300, 100);

  auto *layout = new QVBoxLayout(this);

  QLabel *label = new QLabel("Downloading, please wait...", this);
  m_progressBar = new QProgressBar(this);
  m_progressBar->setRange(0, 100);
  m_progressBar->setValue(0);

  layout->addWidget(label);
  layout->addWidget(m_progressBar);

  setLayout(layout);
}

void DownloaderPrompt::StartDownload(QString repository, QString directory, const QString &contentName, DownloadType type)
{
  if(repository.isEmpty()) return;
  QUrl url(repository);
  bool isRepo = repository.endsWith("/repo", Qt::CaseInsensitive) || repository.endsWith("/content", Qt::CaseInsensitive) ;
  bool isCollection = repository.endsWith("/collection", Qt::CaseInsensitive);
  if(!isRepo && !isCollection)
  {
    QDesktopServices::openUrl(url);
    return;
  }

  url = QUrl(repository += "?key=" + ApiManager::authorizationKey());

  QString downloadText = "Do you want to start the download?";
  if(type == DOWNLOAD_ServerBackground)
  {
    downloadText = "You are missing the background currently being used in the area. Would you like to download it from the workshop?";
  }

  auto reply = QMessageBox::question(
      nullptr,
      "Start Download",
      downloadText,
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
  {
    DownloaderPrompt *prompt = new DownloaderPrompt(nullptr);
    prompt->setDownloadType(type);
    prompt->show();



    QString baseUrl = QString("%1://%2").arg(url.scheme(), url.host());
    if (url.port() != -1) baseUrl += QString(":%1").arg(url.port());

    prompt->setDirectory(directory);
    prompt->setBaseUrl(baseUrl);
    prompt->setIsCollection(isCollection);
    prompt->setContentName(contentName);
    prompt->setRepository(repository);
    prompt->setIsRepo(isRepo);

    QNetworkAccessManager *manager = new QNetworkAccessManager(prompt);

    connect(manager, &QNetworkAccessManager::finished, prompt, &DownloaderPrompt::repoDownloaded);

    manager->get(QNetworkRequest(url));
  }

}

void DownloaderPrompt::ProcessLinks(const QMap<QString, QString>& links, const QString &contentName, const QString& repositoryUrl, bool createContext)
{
  m_cdnFiles.clear();
  m_cdnFiles = links;
  m_progressBar->setValue(0);

  if (m_cdnFiles.isEmpty())
  {
    QMessageBox::information(this, "Download Complete", "All files are already up-to-date!");
    this->deleteLater();
    return;
  }

  m_filesDownloaded = 0;
  m_totalFiles = m_cdnFiles.size();
  m_contentName = contentName;

  QNetworkAccessManager *manager = new QNetworkAccessManager(this);

  for (auto it = m_cdnFiles.constBegin(); it != m_cdnFiles.constEnd(); ++it)
  {
    QString hash = it.value();
    QString filePath = it.key();
    QUrl url(hash);

    QNetworkReply *reply = manager->get(QNetworkRequest(url));

    connect(reply, &QNetworkReply::downloadProgress, this, [this](qint64 bytesReceived, qint64 bytesTotal) {
              if (bytesTotal > 0) {
                qDebug() << "File progress:" << (100 * bytesReceived / bytesTotal) << "%";
              }
            });

    connect(reply, &QNetworkReply::finished, this, [this, reply, filePath, repositoryUrl, createContext]() {
              reply->deleteLater();

              if (reply->error() != QNetworkReply::NoError) {
                QMessageBox::warning(this, "Download Error", reply->errorString());
                return;
              }

              QByteArray data = reply->readAll();

              QFileInfo info(filePath);
              QDir().mkpath(info.absolutePath());

              QFile file(filePath);
              if (file.open(QIODevice::WriteOnly)) {
                file.write(data);
                file.close();
              }

              m_filesDownloaded++;
              int progress = static_cast<int>((double)m_filesDownloaded / m_totalFiles * 100.0);
              m_progressBar->setValue(progress);

              if (m_filesDownloaded == m_totalFiles) {

                FS::Packages::Scan();

                QMessageBox::information(this, "Download Complete", "All files downloaded successfully!");
                this->deleteLater();
                if(m_downloadType == DOWNLOAD_ServerBackground)
                {
                  AOApplication::getInstance()->m_courtroom->update_background_scene();
                }
              }
            });
  }
}

void DownloaderPrompt::repoDownloaded(QNetworkReply *reply)
{
  if (reply->error() != QNetworkReply::NoError)
  {
    QMessageBox::warning(this, "Error", reply->errorString());
    reply->deleteLater();
    return;
  }

  QByteArray response = reply->readAll();
  qDebug() << response;
  QJsonDocument doc = QJsonDocument::fromJson(response);
  QString collectionName = doc["collection_name"].toString();
  QList<QJsonObject> repoObjects = {};

  if(!m_isCollection)
  {
    repoObjects.append(doc.object());
  }
  else
  {
    for(const QJsonValue &val : doc["repos"].toArray())
    {
      repoObjects.append(val.toObject());
    }
  }

  //Cache content into database.


  QMap<QString, QString> hashMap = {};
  for(const QJsonObject& repoObject : repoObjects)
  {
    QString qGUID = repoObject["guid"].toString();
    QString qFolderName = repoObject["folder"].toString();
    QString qDownloadType = repoObject["url_download"].toString();
    int qContentId = repoObject["id"].toInt();
    int qLastUpdated = repoObject["last_updated"].toInt();
    GetDB().cacheContentData(qGUID.toStdString(), qFolderName.toStdString(), qLastUpdated, qContentId);

    QMap<QString, QString> existingFileMap = {};

    if(m_directory == "packages/Workshop Downloads/")
    {
      QString scanDirectory = m_directory + "characters/" + qFolderName + "/";
      existingFileMap = buildFileHashMap(scanDirectory);
    }

    QJsonArray files = repoObject["contents"].toArray();

    for (const QJsonValue &val : files)
    {
      QJsonObject obj = val.toObject();
      QString rawHash = obj["hash"].toString();
      QString hash = m_baseUrl + "/api/workshop/file/" + rawHash;
      QString filePath = "";
      if(qDownloadType == "background")
        filePath = m_directory + "background/" + qFolderName + + "/" + obj["file_path"].toString();
      else
      {
        filePath = m_isCollection ? "packages/" + collectionName + "/" + obj["file_path"].toString() : m_directory + obj["file_path"].toString();
      }

      if(existingFileMap.contains(filePath))
      {
        bool hashMatches = existingFileMap[filePath] == rawHash;
        existingFileMap.remove(filePath);

        if(hashMatches)
          continue;
      }

      hashMap[filePath] = hash;
    }
  }

  ProcessLinks(hashMap, m_contentName, m_repository, m_isRepo);
  reply->deleteLater();
}

