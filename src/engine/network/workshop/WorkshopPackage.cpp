#include "WorkshopPackage.h"


bool WorkshopPackage::extract(const QByteArray &data, const QMap<QString, QString> &files)
{

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

    QString filePath;


    for(auto it = files.constBegin(); it != files.constEnd(); ++it)
    {
      if(it.value().section('/', -1) == hash)
      {
        filePath = it.key();
        break;
      }
    }

    if(filePath.isEmpty())
      continue;


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
  }


  return true;
}
