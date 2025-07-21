#ifndef SCREENSHOT_BUTTON_H
#define SCREENSHOT_BUTTON_H

class ScreenshotButton : public RPButton
{
  Q_OBJECT
public:
  ScreenshotButton(QWidget *parent, AOApplication *p_ao_app);
  void UpdateDimensions();

private slots:
  void OnButtonClicked();
};

#endif // SCREENSHOT_BUTTON_H
