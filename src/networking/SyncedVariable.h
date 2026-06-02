#ifndef SYNCEDVARIABLE_H
#define SYNCEDVARIABLE_H
#include <variant>

enum class SyncedScope
{
  Area,
  Hub,
  User
};

using SyncedValue = std::variant<int, std::string>;

class SyncedVariable
{
public:
  SyncedVariable(const std::string& name);

  SyncedValue get() const;
  void set(const SyncedValue& value);

  void broadcastArea();
  void broadcastHub();
  void broadcastSelf();

private:
  std::string m_name = "";
  SyncedValue m_value;
};

#endif // SYNCEDVARIABLE_H
