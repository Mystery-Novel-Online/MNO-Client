#include "dro_combo_box.h"
#include "commondefs.h"
#include "theme.h"

DROComboBox::DROComboBox(QWidget *parent, AOApplication *p_ao_app)
    : QComboBox(parent)
{
  ao_app = p_ao_app;
}

void DROComboBox::setWidgetInfo(QString widgetName, QString legacyCSS, QString scene)
{
  mLegacyCSSHeader = legacyCSS;
  mWidgetName = widgetName;
  mScene = scene;
}

void DROComboBox::refreshPosition()
{
  set_size_and_pos(this, mWidgetName, COURTROOM_DESIGN_INI, ao_app);
}

void DROComboBox::refreshCSS()
{
  if (!set_stylesheet(this, "[" + mWidgetName + "]", COURTROOM_STYLESHEETS_CSS, ao_app))
  {
    if (!set_stylesheet(this, mLegacyCSSHeader, COURTROOM_STYLESHEETS_CSS, ao_app))
    {
      this->setStyleSheet("");
    }
  }
}
