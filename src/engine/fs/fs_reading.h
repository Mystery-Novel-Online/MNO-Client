#ifndef FS_READING_H
#define FS_READING_H

#include <QString>
#include <QStringList>


namespace FS::Checks
{
  bool FileExists(const QString& path);
  bool DirectoryExists(const QString& path);
  bool CharacterExists(const char* characterFolder);
}

namespace FS::Paths
{
  QString ApplicationPath();
  QString BasePath();
  QString Package(const QString& packageName);

  QString FindFile(const QString& filePath, bool allowPackages = true, const QStringList &extensions = {});
  QString FindFile(const QStringList& filePaths, bool allowPackages = true, const QStringList &extensions = {});

  QStringList FindFiles(const QString& filePath, const QStringList &extensions = {});

  QString FindDirectory(const QString& directoryPath, bool allowPackages = true, bool absolutePath = true);
  QStringList GetDirectoryList(const QString& directoryPath, bool includePackages = true);
  QStringList GetFileList(const QString& directoryPath, bool includePackages = true, const QString& extensionFilter = "", bool includeExtension = false);
  QStringList GetFileList(const QString& directoryPath, const QString& packageName, const QString& extensionFilter = "", bool includeExtension = false);
}

namespace FS::Formats
{
  QStringList SupportedAudio(bool allowExtensionless = false);

  QStringList SupportedImages();
  QStringList AnimatedImages();
  QStringList StaticImages();
}


#endif // FS_READING_H
