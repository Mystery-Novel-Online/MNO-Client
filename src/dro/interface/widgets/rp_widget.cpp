#include "rp_widget.h"
#include "aoapplication.h"

RPWidget::RPWidget(QWidget *parent) : QWidget{parent}
{
  m_App = AOApplication::getInstance();
}
