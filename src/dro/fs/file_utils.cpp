#include "file_utils.h"

#include <QDir>
#include <QFileInfo>


namespace FSChecks
{

  bool FileExists(const QString& path)
  {
    QFileInfo check_file(path);
    return check_file.exists() && check_file.isFile();
  }

  bool DirectoryExists(const QString& path)
  {
    QDir check_dir(path);
    return check_dir.exists();
  }

}

namespace Formats
{

  QStringList SupportedAudio(bool allowExtensionless)
  {
    static QStringList s_ext_list{"", ".wav", ".ogg", ".opus", ".mp3"};
    return allowExtensionless ? s_ext_list.mid(1) : s_ext_list;
  }

  QStringList SupportedImages()
  {
    return AnimatedImages() + StaticImages();
  }

  QStringList AnimatedImages()
  {
    return QStringList{".webp", ".apng", ".gif"};
  }

  QStringList StaticImages()
  {
    return QStringList{".png"};
  }

}

namespace FSPaths
{

QString ApplicationPath()
{
#ifdef Q_OS_MACOS
  QDir l_mac_path(QCoreApplication::applicationDirPath());
  for (int i = 0; i < 3; ++i) // equivalent of "/../../.."
    l_mac_path.cdUp();
  return l_mac_path.canonicalPath();
#else
  return QDir::currentPath();
#endif
}

QString BasePath()
{
  return FSPaths::ApplicationPath() + "/base/";
}

QString PackagePath(QString packageName)
{
  return FSPaths::ApplicationPath() + "/packages/" + packageName + "/";
}


}
