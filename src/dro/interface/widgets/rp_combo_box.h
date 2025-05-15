#ifndef DROCOMBOBOX_H
#define DROCOMBOBOX_H

#include <AOApplication.h>
#include <QComboBox>
#include <QObject>
#include <QWidget>

class RPComboBox : public QComboBox
{
  Q_OBJECT
public:
  RPComboBox(QWidget *parent, AOApplication *p_ao_app);

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
