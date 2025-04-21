#include "fs_writing.h"

#include <QCoreApplication>
#include <QDir>

void DirUtils::CreateInitialFolders()
{
  QDir().mkdir("base/screenshots");
  QDir().mkdir("base/replays");
  QDir().mkdir("base/configs");
}
