#ifndef DROCOMBOBOX_H
#define DROCOMBOBOX_H

#include <AOApplication.h>
#include <QComboBox>
#include <QObject>
#include <QWidget>

class DROComboBox : public QComboBox
{
  Q_OBJECT
public:
  DROComboBox(QWidget *parent, AOApplication *p_ao_app);

  void setWidgetInfo(QString widgetName, QString legacyCSS, QString scene);
  void refreshPosition();
  void refreshCSS();

private:
  QString mLegacyCSSHeader = "[N/A]";
  QString mWidgetName = "FALLBACK";
  QString mScene = "courtroom";

  AOApplication *ao_app = nullptr;
};

#endif // DROCOMBOBOX_H
