#ifndef FS_MOUNTING_H
#define FS_MOUNTING_H

#include <QVector>

namespace FS::Packages
{
  QVector<QString> &Scan();
  QVector<QString> &CachedNames();
  QVector<QString> &DisabledList();
  void SetDisabled(QVector<QString> disableList);
  void SaveDisabled();
}

#endif // FS_MOUNTING_H
