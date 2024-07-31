#include <QStringList>

#pragma once

class QString;

class DRPather
{
public:
  static QString GetApplicationPath();
  static QString GetBasePath();

  static QString SearchPathFirst(QString t_path);
  static QStringList SearchPathAll();

};
