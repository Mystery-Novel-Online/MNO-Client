#ifndef CONFIGURATION_MANAGER_H
#define CONFIGURATION_MANAGER_H

#include <QHash>
#include "drpather.h"

class ConfigurationManager
{
public:
  ConfigurationManager();


private:
  QHash<QString, QString> m_ConfigStringSettings = {};
};

#endif // CONFIGURATION_MANAGER_H
