#include "debug_functions.h"
#include "engine/fs/fs_reading.h"

AONotePicker::AONotePicker(QWidget *p_parent, AOApplication *p_ao_app)
    : QLabel(p_parent)
{
  ao_app = p_ao_app;
}

void AONotePicker::set_active(bool p_active)
{
  m_active = p_active;
}

bool AONotePicker::is_active()
{
  return m_active;
}
