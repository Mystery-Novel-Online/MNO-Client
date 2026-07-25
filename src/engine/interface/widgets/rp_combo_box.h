#ifndef RPCOMBOBOX_H
#define RPCOMBOBOX_H


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

  void setWheelEnabled(bool enabled) { m_wheelEnabled = enabled; }

protected:
  void wheelEvent(QWheelEvent *event) override
  {
    if (m_wheelEnabled)
      QComboBox::wheelEvent(event);
    else
      event->ignore();
  }

private:
  QString mLegacyCSSHeader = "[N/A]";
  QString mWidgetName = "FALLBACK";
  QString mScene = "courtroom";
  bool m_wheelEnabled = true;

  AOApplication *ao_app = nullptr;
};

#endif // DROCOMBOBOX_H
