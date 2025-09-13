#ifndef DOWNLOADER_PROMPT_H
#define DOWNLOADER_PROMPT_H

#include <QObject>
#include <QWidget>

class DownloaderPrompt : public QDialog
{
  Q_OBJECT
public:
  explicit DownloaderPrompt(QWidget *parent = nullptr);
  static void StartDownload(QString repository, QString directory);

  void ProcessLinks(const QMap<QString, QString>& links);

private:
  QProgressBar *m_progressBar;
  QMap<QString, QString> m_cdnFiles = {};

  int m_filesDownloaded;
  int m_totalFiles;

signals:
};

#endif // DOWNLOADER_PROMPT_H
