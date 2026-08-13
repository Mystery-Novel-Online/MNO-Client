#include "HubUploadManager.h"
#include <QHttpPart>

#include <engine/network/api_manager.h>
#include <engine/param/json_reader.h>


HubUploadManager::HubUploadManager(QObject *parent) : QObject(parent) { }

void HubUploadManager::prompt(const QString &packetName)
{
  QString file = QFileDialog::getOpenFileName(nullptr, "Select YAML File", "", "YAML Files (*.yaml)");
  if (file.trimmed().isEmpty())
    return;
  upload(file, packetName);
}

void HubUploadManager::upload(const QString &filePath, const QString &packetName)
{
  auto* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  if(!ApiManager::appendFile(multiPart, "zipfile", filePath))
  {
    emit uploadError("File was unable to be opened.");
    delete multiPart;
    return;
  }

  ApiManager::appendField(multiPart, "key", ApiManager::authorizationKey());
  auto* reply = ApiManager::instance().post("api/hubs/areas/upload", multiPart);

  connect(reply, &QNetworkReply::finished, this,[this, reply, packetName]()
  {
    reply->deleteLater();

    if (reply->error())
    {
      emit uploadError(reply->errorString());
      return;
    }

    JSONReader response;
    response.ReadFromString(reply->readAll());
    AOApplication::getInstance()->send_server_packet(DRPacket(packetName, {response.getStringValue("cdn_file")}));

    emit uploadFinished(packetName, response.getStringValue("cdn_file"));
  });
}
