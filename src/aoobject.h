#pragma once

class AOApplication;

#include <QObject>

class AOObject : public QObject
{
  Q_OBJECT

public:
  AOObject(QObject *p_parent = nullptr);

protected:
  AOApplication *ao_app = nullptr;
};
