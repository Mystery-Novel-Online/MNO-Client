#ifndef RUNTIME_VALUES_H
#define RUNTIME_VALUES_H
#include <QString>
namespace dro::runtime::values
{
  void storeValue(const std::string& name, const std::string& value);
  std::string resolveVariables(const std::string& input);
}
#endif // RUNTIME_VALUES_H
