#ifndef DROLINEEDIT_H
#define DROLINEEDIT_H

#include <AOApplication.h>
#include <QLineEdit>
#include <QWidget>
#include "theme.h"

class DROLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  AOApplication *ao_app = nullptr;
  DROLineEdit(QWidget *parent, AOApplication *p_ao_app);
  void set_theme_settings(QString widgetName, QString legacyCSS, QString scene);

  void setDefaultCSS(QString css);

  void refreshPosition();
  void refreshCSS();

private:
  QString mLegacyCSSHeader = "[N/A]";
  QString mWidgetName = "FALLBACK";
  QString mScene = "courtroom";
  QString mDefaultCSS = "";
};

#endif // DROLINEEDIT_H

