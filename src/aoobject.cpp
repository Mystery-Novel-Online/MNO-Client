#include "aoobject.h"


AOObject::AOObject(QObject *p_parent) : QObject(p_parent)
{
  ao_app = AOApplication::getInstance();
}
