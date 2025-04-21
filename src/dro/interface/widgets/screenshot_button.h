#ifndef SCREENSHOT_BUTTON_H
#define SCREENSHOT_BUTTON_H

#include <QObject>
#include <QWidget>
#include <aobutton.h>

class ScreenshotButton : public AOButton
{
  Q_OBJECT
public:
  ScreenshotButton(QWidget *parent, AOApplication *p_ao_app);
  void UpdateDimensions();

private slots:
  void OnButtonClicked();
};

#endif // SCREENSHOT_BUTTON_H
