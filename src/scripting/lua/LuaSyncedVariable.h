#ifndef LUASYNCEDVARIABLE_H
#define LUASYNCEDVARIABLE_H

#include "networking/SyncedVariable.h"
#include "3rd/sol/sol.hpp"

class LuaSyncedVariable
{
public:
  LuaSyncedVariable(const std::string& name);

  sol::object get(sol::this_state ts) const;
  void set(sol::this_state ts, sol::object value);


private:
  SyncedVariable m_value;
};

#endif // LUASYNCEDVARIABLE_H
