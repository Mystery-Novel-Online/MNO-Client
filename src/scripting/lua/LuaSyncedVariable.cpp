#include "LuaSyncedVariable.h"

LuaSyncedVariable::LuaSyncedVariable(const std::string &name) : m_value(name)
{

}

sol::object LuaSyncedVariable::get(sol::this_state ts) const
{
  sol::state_view lua(ts);

  const auto& value = m_value.get();

  if (std::holds_alternative<int>(value))
    return sol::make_object(lua, std::get<int>(value));

  if (std::holds_alternative<std::string>(value))
    return sol::make_object(lua, std::get<std::string>(value));

  return sol::nil;
}

void LuaSyncedVariable::set(sol::this_state ts, sol::object value)
{
  if (value.is<int>())
  {
    m_value.set(SyncedValue(value.as<int>()));
    std::cout << "VALUE INT SET! " << std::get<int>(m_value.get())  << "\n";
  }
  else if (value.is<std::string>())
  {
    m_value.set(SyncedValue(value.as<std::string>()));
    std::cout << "VALUE STRING SET! " << std::get<std::string>(m_value.get())  << "\n";
  }

}
