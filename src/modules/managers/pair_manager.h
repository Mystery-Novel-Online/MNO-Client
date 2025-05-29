#ifndef PAIRMANAGER_H
#define PAIRMANAGER_H

#include "datatypes.h"
#include <QSlider>
#include <QString>



class PairManager
{
public:
  PairManager(const PairManager&) = delete;

  static PairManager& get()
  {
    return s_Instance;
  }

  void ThemeReload();
  pos_size_type GetElementAlignment(QString name, QString alighment);

private:
  PairManager() {}
  static PairManager s_Instance;
  QHash<QString, pos_size_type> m_chatboxOffsetTransforms = {};
};

#endif // PAIRMANAGER_H
