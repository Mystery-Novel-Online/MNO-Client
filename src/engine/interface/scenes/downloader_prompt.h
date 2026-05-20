#ifndef DOWNLOADER_PROMPT_H
#define DOWNLOADER_PROMPT_H

#include <QObject>
#include <QWidget>
#include "engine/network/workshop/workshop_parser.h"

enum DownloadType
{
  DOWNLOAD_Default,
  DOWNLOAD_ServerBackground,
  DOWNLOAD_HubContent
};

class DownloaderPrompt : public QDialog
{
  Q_OBJECT
public:
  explicit DownloaderPrompt(QWidget *parent = nullptr);
  static void StartDownload(QString repository, QString directory, const QString& contentName, DownloadType type = DOWNLOAD_Default);

  void ProcessLinks(const QMap<QString, QString>& links, const QString& contentName, const QString& repositoryUrl, bool createContext);
  void setDownloadType(DownloadType type) {m_downloadType = type; }

  void setIsRepo(bool state) { m_isRepo = state; }
  void setIsCollection(bool state) { m_isCollection = state; }

  void setContentName(QString name) { m_contentName = name; }
  void setDirectory(QString directory) { m_directory = directory; }
  void setBaseUrl(QString url) { m_baseUrl = url; }
  void setRepository(QString repo) { m_repository = repo; }


private:
  QProgressBar *m_progressBar;
  QMap<QString, QString> m_cdnFiles = {};

  QString m_contentName = "";
  int m_filesDownloaded;
  int m_totalFiles;

  DownloadType m_downloadType = DOWNLOAD_Default;

  QString m_directory = "";
  QString m_baseUrl = "";
  QString m_repository = "";

  bool m_isRepo = false;
  bool m_isCollection = false;


public slots:
  void repoDownloaded(QNetworkReply *reply);

};

#endif // DOWNLOADER_PROMPT_H
