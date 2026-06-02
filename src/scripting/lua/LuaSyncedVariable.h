#ifndef LUASYNCEDVARIABLE_H
#define LUASYNCEDVARIABLE_H

#include "networking/SyncedVariable.h"
#include "3rd/sol/sol.hpp"

class LuaSyncedVariable
{
public:
  LuaSyncedVariable(const std::string& name, SyncedScope scope);

  sol::object get(sol::this_state ts) const;
  void set(sol::this_state ts, sol::object value);


private:
  SyncedVariable m_value;
  SyncedScope m_scope;
};

#endif // LUASYNCEDVARIABLE_H
