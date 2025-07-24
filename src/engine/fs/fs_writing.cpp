#include "fs_writing.h"

void DirUtils::CreateInitialFolders()
{
  QDir().mkdir("base/screenshots");
  QDir().mkdir("base/replays");
  QDir().mkdir("base/configs");
}
