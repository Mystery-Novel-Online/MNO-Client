#include "dir_utils.h"

#include <QCoreApplication>
#include <QDir>

void DirUtils::CreateInitialFolders()
{
  QDir().mkdir("base/screenshots");
  QDir().mkdir("base/replays");
  QDir().mkdir("base/configs");
}

QString DirUtils::GetApplicationPath()
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
