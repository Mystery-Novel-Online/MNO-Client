#ifndef HUBUPLOADMANAGER_H
#define HUBUPLOADMANAGER_H

class HubUploadManager : public QObject
{
  Q_OBJECT
public:
  explicit HubUploadManager(QObject* parent = nullptr);

  void prompt(const QString& packetName);
  void upload(const QString& filePath, const QString& packetName);

signals:
  void uploadFinished(const QString& packetName, const QString& cdnHash);
  void uploadError(const QString& error);
};

#endif // HUBUPLOADMANAGER_H
