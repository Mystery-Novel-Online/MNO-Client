#include "characterselectwidget.h"

#include <QPaintEvent>
#include "courtroom.h"

CharacterSelectWidget::CharacterSelectWidget(QWidget *parent, AOApplication *p_ao_app) : QWidget(parent)
{
  ao_app = p_ao_app;
}

void CharacterSelectWidget::wheelEvent(QWheelEvent *event)
{
  int delta = event->angleDelta().y();

  if (delta > 0)
  {
    ao_app->get_courtroom()->on_char_select_left_clicked();
  }
  else if (delta < 0)
  {
    ao_app->get_courtroom()->on_char_select_right_clicked();
  }

  QWidget::wheelEvent(event);

}
