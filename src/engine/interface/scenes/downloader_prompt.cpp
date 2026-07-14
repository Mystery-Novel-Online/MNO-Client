#include "downloader_prompt.h"
#include "engine/fs/fs_characters.h"
#include "engine/fs/fs_mounting.h"
#include "engine/system/user_database.h"

#include <engine/network/api_manager.h>
#include "utils/file_hash_util.h"
#include <rolechat/util/FileSystem.h>
#include <rolechat/filesystem/RCDir.h>

DownloaderPrompt::DownloaderPrompt(QWidget *parent) : QDialog{parent}
{
  setWindowTitle("Downloading...");
  setModal(true);
  setFixedSize(400, 100);

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

  std::optional<int> originType;

  switch(type)
  {
  case DOWNLOAD_Discord:
    originType = 4;
    break;

  case DOWNLOAD_PlayerList:
    originType = 2;
    break;

  case DOWNLOAD_Workshop:
    originType = 0;
    break;

  default:
    originType.reset();
    break;
  }

  QString repoUrl = repository += "?key=" + ApiManager::authorizationKey();

  if(originType.has_value())
    repoUrl += "&origin=" + QString::number(originType.value());

  url = QUrl(repoUrl);

  QString downloadText = "Do you want to start the download?";
  switch(type)
  {
    case DOWNLOAD_ServerBackground:
      downloadText = "You are missing the background currently being used in the area. Would you like to download it from the workshop?";
    default:
      break;
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

bool DownloaderPrompt::StartDownload(const QStringList &guids, DownloadType type)
{
  if(guids.empty())
    return false;

  QString urlString = ApiManager::baseUri() + "api/workshop/multi";

  std::optional<int> originType;

  switch(type)
  {
  case DOWNLOAD_ServerContent:
    originType = 5;
    break;

  default:
    originType.reset();
    break;
  }

  urlString += "?key=" + ApiManager::authorizationKey();

  if(originType.has_value())
    urlString += "&origin=" + QString::number(originType.value());

  QUrl url(urlString);

  QString downloadText = "Do you want to start the download?";
  switch(type)
  {
    case DOWNLOAD_ServerContent:
      downloadText = "The content this server recommends is either not installed or is out-of-date.\nWould you like download the missing files?";
    default:
      break;
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

    prompt->setBaseUrl(baseUrl);

    QNetworkAccessManager *manager = new QNetworkAccessManager(prompt);

    connect(manager, &QNetworkAccessManager::finished, prompt, &DownloaderPrompt::repoDownloaded);

    QJsonObject body;
    body["guids"] = QJsonArray::fromStringList(guids);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    manager->post(request, QJsonDocument(body).toJson());

    return true;
  }

  return false;

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

    connect(reply, &QNetworkReply::downloadProgress, this,
            [this, reply](qint64 bytesReceived, qint64 bytesTotal)
            {
              qint64 &prev = m_replyProgress[reply];

              qint64 delta = bytesReceived - prev;
              prev = bytesReceived;

              m_downloadedBytes += delta;

              updateUi();
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

              if (m_filesDownloaded == m_totalFiles) {

                rolechat::fs::PackageManager::scanPackages();

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

void DownloaderPrompt::updateUi()
{
  if (m_downloadedBytes <= 0) return;

  int progress = static_cast<int>((double)m_downloadedBytes / m_totalDownloadBytes * 100.0);
  m_progressBar->setValue(progress);

  if (!m_speedTimer.isValid())
    m_speedTimer.start();

  qint64 elapsedMs = m_speedTimer.elapsed();
  if (elapsedMs > 0)
  {
    m_currentSpeed = (double)(m_downloadedBytes - m_lastBytes) / (elapsedMs / 1000.0);
    m_lastBytes = m_downloadedBytes;
    m_speedTimer.restart();
  }

  setWindowTitle(
      QString("Downloading... %2 / %3 | %1/s")
          .arg(formatBytes(m_currentSpeed))
          .arg(formatBytes(m_downloadedBytes))
          .arg(formatBytes(m_totalDownloadBytes))
      );
}

QString DownloaderPrompt::formatBytes(double bytes)
{
  const char* units[] = {"B", "KB", "MB", "GB", "TB"};

  int i = 0;
  while (bytes >= 1024.0 && i < 4)
  {
    bytes /= 1024.0;
    ++i;
  }

  return QString("%1 %2")
      .arg(bytes, 0, 'f', (i == 0) ? 0 : 1)
      .arg(units[i]);
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

  if(response.startsWith('['))
  {
    m_currentCollection = WorkshopParser::parseCollections(response);
  }
  else
  {
    m_currentCollection.append(WorkshopParser::parseCollection(response));
  }

  QMap<QString, QString> hashMap = {};

  for(auto& collection : m_currentCollection)
  {

    QString packageDirectory = collection.packageDirectory();
    m_totalDownloadBytes += (double)collection.sizeBytes;


    for (const WorkshopRepository& repo : collection.repositories)
    {
      QString scanDirectory = "";
      QMap<QString, QString> existingFileMap = {};

      if(repo.downloadType == "repo")
      {
        scanDirectory = packageDirectory + "characters/" + repo.folderName + "/";

        auto workshopSearch = GetDB().searchContentGuid(repo.guid.toStdString());
        if(!workshopSearch.folder.empty())
        {
          QString existingPath = QString::fromStdString(rolechat::fs::RCDir("characters/" + workshopSearch.folder).findFirst());

          if (!QDir(scanDirectory).exists())
          {
            if (QDir().rename(existingPath, scanDirectory))
            {
              qDebug() << "Moved successfully";
            }
          }
        }

        existingFileMap = FileHashUtil::buildMd5Map(scanDirectory);
      }

      GetDB().cacheContentData(repo.guid.toStdString(), repo.folderName.toStdString(), repo.lastUpdated, repo.contentId);

      for (const WorkshopFile& file : repo.files)
      {
        QString cdnUri = m_baseUrl + "/api/workshop/file/" + file.hash;
        QString filePath = "";

        if(repo.downloadType == "background")
          filePath = packageDirectory + "background/" + repo.folderName + + "/" + file.relativePath;
        else
          filePath = packageDirectory + file.relativePath;

        if(!filePath.startsWith(scanDirectory))
        {
          QFile scanningFile(filePath);
          if (scanningFile.exists())
          {
            existingFileMap[filePath] = FileHashUtil::md5File(filePath);
            m_downloadedBytes += scanningFile.size();
          }
        }

        if(existingFileMap.contains(filePath))
        {
          QFile scanningFile(filePath);
          m_downloadedBytes += scanningFile.size();

          bool hashMatches = existingFileMap[filePath] == file.hash;
          existingFileMap.remove(filePath);

          if(hashMatches)
            continue;
        }

        hashMap[filePath] = cdnUri;
      }

      for (auto it = existingFileMap.begin(); it != existingFileMap.end(); ++it)
      {
        const QString &staleFilePath = it.key();

        QFile file(staleFilePath);
        if (file.exists())
        {
          file.remove();
        }
      }

    }
  }


  int progress = static_cast<int>((double)m_downloadedBytes / m_totalDownloadBytes * 100.0);
  m_progressBar->setValue(progress);

  ProcessLinks(hashMap, m_contentName, m_repository, m_isRepo);
  reply->deleteLater();
}

