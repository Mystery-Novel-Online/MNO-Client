#include "rp_combo_box.h"
#include "dro/system/theme.h"
#include "theme.h"

RPComboBox::RPComboBox(QWidget *parent, AOApplication *p_ao_app)
    : QComboBox(parent)
{
  ao_app = p_ao_app;
}

void RPComboBox::setWidgetInfo(QString widgetName, QString legacyCSS, QString scene)
{
  mLegacyCSSHeader = legacyCSS;
  mWidgetName = widgetName;
  mScene = scene;
}

void RPComboBox::refreshPosition()
{
  dro::system::theme::applyDimensions(this, mWidgetName, SceneType_Courtroom);
}

void RPComboBox::refreshCSS()
{
  if (!set_stylesheet(this, "[" + mWidgetName + "]", COURTROOM_STYLESHEETS_CSS, ao_app))
  {
    if (!set_stylesheet(this, mLegacyCSSHeader, COURTROOM_STYLESHEETS_CSS, ao_app))
    {
      this->setStyleSheet("");
    }
  }
}
