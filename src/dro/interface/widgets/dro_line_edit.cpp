#include "dro_line_edit.h"
#include "commondefs.h"

DROLineEdit::DROLineEdit(QWidget *parent, AOApplication *p_ao_app)
    : QLineEdit(parent)
{
  ao_app = p_ao_app;
}

void DROLineEdit::set_theme_settings(QString widgetName, QString legacyCSS, QString scene)
{
  mLegacyCSSHeader = legacyCSS;
  mWidgetName = widgetName;
  mScene = scene;
}

void DROLineEdit::setDefaultCSS(QString css)
{
  mDefaultCSS = css;
}

void DROLineEdit::refreshPosition()
{
  set_size_and_pos(this, mWidgetName, COURTROOM_DESIGN_INI, ao_app);
  set_text_alignment(this, mWidgetName, COURTROOM_FONTS_INI, ao_app);
}

void DROLineEdit::refreshCSS()
{
  if (!set_stylesheet(this, "[" + mWidgetName + "]", COURTROOM_STYLESHEETS_CSS, ao_app))
  {
    if (!set_stylesheet(this, mLegacyCSSHeader, COURTROOM_STYLESHEETS_CSS, ao_app))
    {
      if(!mDefaultCSS.isEmpty()) this->setStyleSheet(mDefaultCSS);
    }
  }
}
