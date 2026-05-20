#ifndef FILE_HASH_UTIL_H
#define FILE_HASH_UTIL_H

class FileHashUtil
{
public:
  static QMap<QString, QString> buildMd5Map(const QString& directory);
  static QString md5File(const QString& filePath);
};

#endif // FILE_HASH_UTIL_H
