#include "WorkshopPackage.h"


bool WorkshopPackage::extract(const QByteArray &data, const QMap<QString, QString> &files)
{
  QHash<QString, QStringList> hashToPaths;

  for(auto it = files.constBegin(); it != files.constEnd(); ++it)
  {
    QString hash = it.value().section('/', -1);
    hashToPaths[hash].append(it.key());
  }

  QDataStream stream(data);
  stream.setByteOrder(QDataStream::LittleEndian);

  quint32 fileCount;
  stream >> fileCount;

  for(quint32 i = 0; i < fileCount; i++)
  {
    quint16 hashLength;
    stream >> hashLength;

    QByteArray hashBytes(hashLength, Qt::Uninitialized);

    stream.readRawData(hashBytes.data(), hashLength);

    QString hash = QString::fromUtf8(hashBytes);

    quint64 fileSize;
    stream >> fileSize;

    QByteArray fileData(fileSize, Qt::Uninitialized);

    stream.readRawData(fileData.data(), fileSize);

    auto pathIt = hashToPaths.find(hash);

    if(pathIt == hashToPaths.end())
    {
      qWarning() << "Unknown hash:" << hash;
      continue;
    }

    for(const QString& filePath : pathIt.value())
    {
      QFileInfo info(filePath);

      QDir().mkpath(info.absolutePath());

      QFile file(filePath);

      if(!file.open(QIODevice::WriteOnly))
      {
        qWarning() << "Failed writing:" << filePath;
        continue;
      }

      file.write(fileData);
      file.close();

      qDebug() << "Extracted:" << filePath;
    }

    hashToPaths.erase(pathIt);
  }


  return true;
}
