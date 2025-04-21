#include "fs_reading.h"
#include "fs_mounting.h"

#include <QDir>
#include <QFileInfo>


namespace FS::Checks
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

namespace FS::Formats
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

namespace FS::Paths
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
  return FS::Paths::ApplicationPath() + "/base/";
}

QString Package(const QString& packageName)
{
  return FS::Paths::ApplicationPath() + "/packages/" + packageName + "/";
}

QString FindFile(const QString &filePath, bool allowPackages)
{
  if(!allowPackages) return BasePath() + filePath;

  QVector<QString> packageNames = Packages::CachedNames();
  QVector<QString> disabledList = Packages::DisabledList();

  for(QString &packageName : packageNames)
  {
    if(!disabledList.contains(packageName))
    {
      QString package_path = Paths::Package(packageName) + filePath;
      if(Checks::FileExists(package_path))
      {
        return package_path;
      }
    }
  }

  return BasePath() + filePath;
}

QString FindDirectory(const QString &directoryPath, bool allowPackages)
{
  if(!allowPackages) return BasePath() + directoryPath;

  QVector<QString> packageNames = Packages::CachedNames();
  QVector<QString> disabledList = Packages::DisabledList();

  for (int i=0; i< packageNames.size(); i++)
  {
    if(!disabledList.contains(packageNames.at(i)))
    {
      QString packagePath = Paths::Package(packageNames.at(i)) + directoryPath;
      if(Checks::DirectoryExists(packagePath))
      {
        return packagePath;
      }
    }
  }

  return BasePath() + directoryPath;
}


}
