#ifndef DIR_UTILS_H
#define DIR_UTILS_H

#include <QString>


class DirUtils
{
public:
  static void CreateInitialFolders();
  static QString GetApplicationPath();
};

#endif // DIR_UTILS_H
