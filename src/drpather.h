#include <QStringList>

#pragma once

class QString;

class DRPather
{
public:
  static QString GetApplicationPath();
  static QString GetBasePath(QString t_subpath = "");


  static QString SearchPathFirst(QString t_path);
  static QStringList SearchPathAll();

};
