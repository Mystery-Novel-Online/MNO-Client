#include "downloader_prompt.h"
#include "engine/fs/fs_characters.h"
#include "engine/fs/fs_mounting.h"
#include "engine/system/user_database.h"

#include <engine/network/api_manager.h>
#include "utils/file_hash_util.h"
#include <rolechat/util/FileSystem.h>

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

void DownloaderPrompt::repoDownloaded(QNetworkReply *reply)
{
  if (reply->error() != QNetworkReply::NoError)
  {
    QMessageBox::warning(this, "Error", reply->errorString());
    reply->deleteLater();
    return;
  }

  QByteArray response = reply->readAll();
  WorkshopCollection collection = WorkshopParser::parseCollection(response);
  QString packageDirectory = collection.packageDirectory();


  QMap<QString, QString> hashMap = {};

  for (const WorkshopRepository& repo : collection.repositories)
  {
    GetDB().cacheContentData(repo.guid.toStdString(), repo.folderName.toStdString(), repo.lastUpdated, repo.contentId);
    QMap<QString, QString> existingFileMap = {};
    QString scanDirectory = "";

    if(repo.downloadType == "repo")
    {
      scanDirectory = packageDirectory + "characters/" + repo.folderName + "/";
      existingFileMap = FileHashUtil::buildMd5Map(scanDirectory);
    }

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
        if (QFile(filePath).exists())
        {
          existingFileMap[filePath] = FileHashUtil::md5File(filePath);
        }
      }

      if(existingFileMap.contains(filePath))
      {
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

  ProcessLinks(hashMap, m_contentName, m_repository, m_isRepo);
  reply->deleteLater();
}

