#include "file_hash_util.h"

QMap<QString, QString> FileHashUtil::buildMd5Map(const QString &directory)
{
  QMap<QString, QString> fileHashMap;

  QDirIterator it(directory, QDir::Files, QDirIterator::Subdirectories);

  while (it.hasNext())
  {
    QString filePath = it.next();
    fileHashMap.insert(filePath, md5File(filePath));
  }

  return fileHashMap;
}

QString FileHashUtil::md5File(const QString &filePath)
{
  QFile file(filePath);

  if (!file.open(QIODevice::ReadOnly))
    return {};

  return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
}
