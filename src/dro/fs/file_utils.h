#ifndef FILE_UTILS_H
#define FILE_UTILS_H

class QString;
class QStringList;


namespace FSChecks
{
  bool FileExists(const QString& path);
  bool DirectoryExists(const QString& path);
}

namespace FSPaths
{
  QString ApplicationPath();
  QString BasePath();
  QString PackagePath(QString packageName);
}

namespace Formats
{
  QStringList SupportedAudio(bool allowExtensionless = false);

  QStringList SupportedImages();
  QStringList AnimatedImages();
  QStringList StaticImages();
}


#endif // FILE_UTILS_H
