#ifndef SCREENSHOT_MENU_H
#define SCREENSHOT_MENU_H

#include <QMenu>

class ScreenshotMenu : public QMenu
{
  Q_OBJECT
public:
  ScreenshotMenu();

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnCaptureLog();
  void OnCaptureWindow();
  void OnDirectoryOpen();

private:
  QAction *m_CaptureLogAction;
  QAction *m_CaptureWindowAction;
  QAction *m_OpenCapturesAction;

  Courtroom *parentCourtroom;
};

#endif // SCREENSHOT_MENU_H
