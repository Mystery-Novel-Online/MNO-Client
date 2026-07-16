#ifndef CONTENTDOWNLOADER_H
#define CONTENTDOWNLOADER_H

#include <QObject>
#include <QMap>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

class ContentDownloader : public QObject
{
  Q_OBJECT
public:
  explicit ContentDownloader(QObject* parent = nullptr);

  void download(const QMap<QString, QString>& files, int fileSize = 0);

signals:
  void progressChanged(int percent);

  void downloadFinished(QByteArray data);

  void downloadFailed(QString error);


private:
  QNetworkAccessManager* m_manager;
};

#endif // CONTENTDOWNLOADER_H
