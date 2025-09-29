#ifndef DOWNLOADER_PROMPT_H
#define DOWNLOADER_PROMPT_H

#include <QObject>
#include <QWidget>

class DownloaderPrompt : public QDialog
{
  Q_OBJECT
public:
  explicit DownloaderPrompt(QWidget *parent = nullptr);
  static void StartDownload(QString repository, QString directory, const QString& contentName);

  void ProcessLinks(const QMap<QString, QString>& links, const QString& contentName, const QString& repositoryUrl, bool createContext);


private:
  QProgressBar *m_progressBar;
  QMap<QString, QString> m_cdnFiles = {};

  QString m_contentName = "";
  int m_filesDownloaded;
  int m_totalFiles;

signals:
};

#endif // DOWNLOADER_PROMPT_H
