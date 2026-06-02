#include "SyncedVariable.h"


SyncedVariable::SyncedVariable(const std::string &name) : m_name(name)
{

}

SyncedValue SyncedVariable::get() const
{
  return m_value;
}

void SyncedVariable::set(const SyncedValue &value)
{
  m_value = value;
}
