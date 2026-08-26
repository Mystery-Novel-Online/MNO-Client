#include "file_hash_util.h"

QMap<QString, QString> FileHashUtil::buildMd5Map(const QString &a_directoryPath) {
  QMap<QString, QString> fileHashMap;

  QDirIterator it(a_directoryPath, QDir::Files, QDirIterator::Subdirectories);

  while(it.hasNext()) {
    QString filePath = it.next();
    fileHashMap.insert(filePath, md5File(filePath));
  }

  return fileHashMap;
}

QString FileHashUtil::md5File(const QString &a_filePath) {
  QFile file(a_filePath);

  if(!file.open(QIODevice::ReadOnly)) {
    return {};
  }

  return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
}
