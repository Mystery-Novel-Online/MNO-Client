#ifndef DOWNLOADER_PROMPT_H
#define DOWNLOADER_PROMPT_H

#include <QObject>
#include <QWidget>

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


private:
  QProgressBar *m_progressBar;
  QMap<QString, QString> m_cdnFiles = {};

  QString m_contentName = "";
  int m_filesDownloaded;
  int m_totalFiles;

  DownloadType m_downloadType = DOWNLOAD_Default;

signals:
};

#endif // DOWNLOADER_PROMPT_H
