#include "ContentDownloader.h"
#include "engine/network/api_manager.h"

ContentDownloader::ContentDownloader(QObject *parent)
{
  m_manager = new QNetworkAccessManager(this);
}

void ContentDownloader::download(const QMap<QString, QString> &files, int fileSize)
{
  QJsonArray hashes;


  for(auto it = files.constBegin(); it != files.constEnd(); ++it)
  {
    QString hash = it.value().section('/', -1);
    hashes.append(hash);
  }

  QJsonObject json;
  json["hashes"] = hashes;

  QNetworkRequest request(QUrl(ApiManager::baseUri() + "api/workshop/download"));

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json" );

  QNetworkReply* reply = m_manager->post(request, QJsonDocument(json).toJson());


  connect(reply, &QNetworkReply::downloadProgress, this, [this, fileSize](qint64 received, qint64 total)
  {
    if(fileSize > 0)
    {
      emit progressChanged( (received * 100) / fileSize);
    }
  });

  connect(reply, &QNetworkReply::finished, this, [this, reply]()
  {
    if(reply->error() != QNetworkReply::NoError)
    {
      emit downloadFailed(
          reply->errorString()
          );

      reply->deleteLater();
      return;
    }

    emit downloadFinished(reply->readAll());

    reply->deleteLater();
  });
}
