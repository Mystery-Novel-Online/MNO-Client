#include "fs_reading.h"
#include "fs_mounting.h"

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

  bool CharacterExists(const char *characterFolder)
  {
    return DirectoryExists(Paths::FindDirectory("characters/" + QString(characterFolder)));
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

QString FindFile(const QString &filePath, bool allowPackages, const QStringList &extensions)
{
  if(!allowPackages) return BasePath() + filePath;

  QStringList appendedFileList = {filePath};

  for(const QString &extension : extensions)
  {
    appendedFileList.append(filePath + extension);
  }

  QVector<QString> packageNames = Packages::CachedNames();
  QVector<QString> disabledList = Packages::DisabledList();

  for(QString &packageName : packageNames)
  {
    if(!disabledList.contains(packageName))
    {
      for(const QString& path : appendedFileList)
      {
        QString package_path = Paths::Package(packageName) + path;
        if(Checks::FileExists(package_path))
        {
          return package_path;
        }
      }
    }
  }

  for(const QString& path : appendedFileList)
  {
    QString baseFilePath = BasePath() + path;
    if(Checks::FileExists(baseFilePath))
    {
      return baseFilePath;
    }
  }
  return BasePath() + filePath;
}

QString FindDirectory(const QString &directoryPath, bool allowPackages, bool absolutePath)
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

  if(absolutePath) return BasePath() + directoryPath;
  return "base/" + directoryPath;
}

QStringList GetFileList(const QString &directoryPath, bool includePackages, const QString& extensionFilter, bool includeExtension)
{
  QStringList returnValues = {};

  QDir targetDirectory("base/" + directoryPath);
  QStringList fileList = targetDirectory.entryList(QStringList() << "*." + extensionFilter, QDir::Files);

  for (const QString &fileName : fileList)
  {
    if (includeExtension)
      returnValues.append(fileName);
    else
      returnValues.append(fileName.left(fileName.lastIndexOf('.')));
  }

  if(includePackages)
  {
    QVector<QString> searchArchives = Packages::CachedNames();
    for(const QString& packageName : searchArchives)
    {
      QDir targetDirectory("packages/" + packageName + "/" + directoryPath);
      QStringList fileList = targetDirectory.entryList(QStringList() << "*." + extensionFilter, QDir::Files);
      for (const QString &fileName : fileList)
      {
        if (includeExtension)
          returnValues.append(fileName);
        else
          returnValues.append(fileName.left(fileName.lastIndexOf('.')));
      }
    }
  }

  return returnValues;
}

QStringList GetDirectoryList(const QString &directoryPath, bool includePackages)
{
  QStringList returnValues = {};

  QDir targetDirectory("base/" + directoryPath);
  QStringList subDirectories = targetDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  for (const QString &directoryName : subDirectories)
  {
    returnValues.append(directoryName);
  }

  if(includePackages)
  {
    //TO-DO
    QVector<QString> searchArchives = Packages::CachedNames();
  }

  return returnValues;
}

QStringList GetFileList(const QString &directoryPath, const QString &packageName, const QString &extensionFilter, bool includeExtension)
{
  QStringList returnValues = {};

  QDir targetDirectory("packages/" + packageName + "/" + directoryPath);
  QStringList fileList = targetDirectory.entryList(QStringList() << "*." + extensionFilter, QDir::Files);

  for (const QString &fileName : fileList)
  {
    if (includeExtension)
      returnValues.append(fileName);
    else
      returnValues.append(fileName.left(fileName.lastIndexOf('.')));
  }

  return returnValues;
}

QString FindFile(const QStringList &filePaths, bool allowPackages, const QStringList &extensions)
{
  QStringList allCandidatePaths;

  for (const QString &filePath : filePaths)
  {
    allCandidatePaths.append(filePath);
    for (const QString &extension : extensions)
    {
      allCandidatePaths.append(filePath + extension);
    }
  }

  if (allowPackages)
  {
    QVector<QString> packageNames = Packages::CachedNames();
    QVector<QString> disabledList = Packages::DisabledList();

    for (const QString &packageName : packageNames)
    {
      if (!disabledList.contains(packageName))
      {
        for (const QString &path : allCandidatePaths)
        {
          QString packagePath = Paths::Package(packageName) + path;
          if (Checks::FileExists(packagePath))
          {
            return packagePath;
          }
        }
      }
    }
  }

  for (const QString &path : allCandidatePaths)
  {
    QString basePath = BasePath() + path;
    if (Checks::FileExists(basePath))
    {
      return basePath;
    }
  }

  return "";
}

QStringList FindFiles(const QString &filePath, const QStringList &extensions)
{
  QStringList returnValues = {};


  QStringList appendedFileList = {};

  if(extensions.count() > 0)
  {
    for(const QString &extension : extensions)
      appendedFileList.append(filePath + extension);
  }
  else
  {
    appendedFileList.append(filePath);
  }

  for(const QString& path : appendedFileList)
  {
    QString baseFilePath = BasePath() + path;
    if(Checks::FileExists(baseFilePath))
    {
      returnValues.append(baseFilePath);
    }
  }

  QVector<QString> packageNames = Packages::CachedNames();
  QVector<QString> disabledList = Packages::DisabledList();

  for(QString &packageName : packageNames)
  {
    if(!disabledList.contains(packageName))
    {
      for(const QString& path : appendedFileList)
      {
        QString package_path = Paths::Package(packageName) + path;
        if(Checks::FileExists(package_path))
        {
          returnValues.append(package_path);
        }
      }
    }
  }



  return returnValues;
}


}
