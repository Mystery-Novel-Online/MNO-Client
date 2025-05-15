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
  DROLineEdit(const QString &name, const QString &placeholder, const QString &cssHeader, QWidget *parent);
  DROLineEdit(const QString &name, const QString &cssHeader, QWidget *parent);

  void set_theme_settings(QString widgetName, QString legacyCSS, QString scene);

  void setDefaultCSS(QString css);

  void refreshPosition();
  void refreshCSS();

private:
  AOApplication *m_app = nullptr;
  QString m_friendlyName = "widget";
  QString m_cssHeader = "[default]";
  QString m_targetScene = "courtroom";
  QString m_fallbackCSS = "";
};

#endif // DROLINEEDIT_H

