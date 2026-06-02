#ifndef SYNCEDVARIABLE_H
#define SYNCEDVARIABLE_H
#include <variant>

using SyncedValue = std::variant<int, std::string>;

class SyncedVariable
{
public:
  SyncedVariable(const std::string& name);

  SyncedValue get() const;
  void set(const SyncedValue& value);

private:
  std::string m_name = "";
  SyncedValue m_value;
};

#endif // SYNCEDVARIABLE_H
