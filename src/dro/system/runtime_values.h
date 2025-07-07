#ifndef RUNTIME_VALUES_H
#define RUNTIME_VALUES_H
#include <QString>
namespace dro::runtime::values
{
  void storeValue(const QString& name, const QString& value);
  QString resolveVariables(const QString& input);
}
#endif // RUNTIME_VALUES_H
