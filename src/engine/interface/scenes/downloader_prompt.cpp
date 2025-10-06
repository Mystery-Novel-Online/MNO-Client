#include "downloader_prompt.h"
#include "engine/fs/fs_characters.h"
#include "engine/fs/fs_mounting.h"
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

void DownloaderPrompt::StartDownload(QString repository, QString directory, const QString &contentName)
{
  if(repository.isEmpty()) return;
  QUrl url(repository);
  bool isRepo = repository.endsWith("/repo", Qt::CaseInsensitive);
  bool isCollection = repository.endsWith("/collection", Qt::CaseInsensitive);
  if(!isRepo && !isCollection)
  {
    QDesktopServices::openUrl(url);
    return;
  }

  url = QUrl(repository += "?key=" + QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE")));

  auto reply = QMessageBox::question(
      nullptr,
      "Start Download",
      "Do you want to start the download?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes)
  {
    DownloaderPrompt *prompt = new DownloaderPrompt(nullptr);
    prompt->show();

    QString baseUrl = QString("%1://%2").arg(url.scheme(), url.host());
    if (url.port() != -1) baseUrl += QString(":%1").arg(url.port());

    QNetworkAccessManager *manager = new QNetworkAccessManager(prompt);
    QObject::connect(manager, &QNetworkAccessManager::finished,[prompt, directory, baseUrl, isCollection, contentName, repository, isRepo](QNetworkReply *reply) {
                       if (reply->error() != QNetworkReply::NoError)
                       {
                         QMessageBox::warning(prompt, "Error", reply->errorString());
                         reply->deleteLater();
                         return;
                       }

                       QByteArray response = reply->readAll();
                       QJsonDocument doc = QJsonDocument::fromJson(response);
                       if (!doc.isArray() && !isCollection)
                       {
                         QMessageBox::warning(prompt, "Error", "Invalid repo format");
                         reply->deleteLater();
                         return;
                       }
                       QString collectionName = doc["collection_name"].toString();

                       QJsonArray files;
                       if(isCollection)
                       {
                         files = doc["contents"].toArray();
                       }
                        else
                       {
                         files = doc.array();
                       }
                       QMap<QString, QString> hashMap = {};
                       for (const QJsonValue &val : files)
                       {
                         QJsonObject obj = val.toObject();
                         QString hash = baseUrl + "/api/workshop/file/" + obj["hash"].toString();
                         QString filePath = isCollection ? "packages/" + collectionName + "/" + obj["file_path"].toString() : directory + "/" + obj["file_path"].toString();
                         hashMap[filePath] = hash;
                       }
                       prompt->ProcessLinks(hashMap, contentName, repository, isRepo);
                       reply->deleteLater();
                     });

    manager->get(QNetworkRequest(url));
  }

}

void DownloaderPrompt::ProcessLinks(const QMap<QString, QString>& links, const QString &contentName, const QString& repositoryUrl, bool createContext)
{
  m_cdnFiles.clear();
  m_cdnFiles = links;
  m_progressBar->setValue(0);

  if (m_cdnFiles.isEmpty())
    return;

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

                if(createContext)
                {
                  QString contentFilePath = fs::characters::getFilePath(m_contentName, "CONTENT.txt");
                  QFile contentFile(contentFilePath);
                  if (contentFile.open(QIODevice::WriteOnly | QIODevice::Text))
                  {
                    QUrl url(repositoryUrl);
                    url.setQuery(QString());

                    QTextStream out(&contentFile);
                    out << url.toString();
                    contentFile.close();
                  }
                }

                FS::Packages::Scan();

                QMessageBox::information(this, "Download Complete", "All files downloaded successfully!");
                this->deleteLater();
              }
            });
  }
}

