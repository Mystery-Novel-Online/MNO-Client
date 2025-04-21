#ifndef FS_READING_H
#define FS_READING_H

class QString;
class QStringList;


namespace FS::Checks
{
  bool FileExists(const QString& path);
  bool DirectoryExists(const QString& path);
}

namespace FS::Paths
{
  QString ApplicationPath();
  QString BasePath();
  QString Package(const QString& packageName);
  QString FindFile(const QString& filePath, bool allowPackages = true);
  QString FindDirectory(const QString& directoryPath, bool allowPackages = true);
}

namespace FS::Formats
{
  QStringList SupportedAudio(bool allowExtensionless = false);

  QStringList SupportedImages();
  QStringList AnimatedImages();
  QStringList StaticImages();
}


#endif // FS_READING_H
