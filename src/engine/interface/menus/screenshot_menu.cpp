#include "screenshot_menu.h"
#include "engine/system/audio.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/fs/fs_reading.h"

#include <modules/theme/thememanager.h>
ScreenshotMenu::ScreenshotMenu()
{
  m_CaptureLogAction = addAction(tr("Capture IC Log"));
  m_CaptureWindowAction = addAction(tr("Capture Entire Window"));
  m_OpenCapturesAction = addAction(tr("Open Screenshot Directory"));

  connect(m_CaptureLogAction, &QAction::triggered, this, &ScreenshotMenu::OnCaptureLog);
  connect(m_CaptureWindowAction, &QAction::triggered, this, &ScreenshotMenu::OnCaptureWindow);
  connect(m_OpenCapturesAction, &QAction::triggered, this, &ScreenshotMenu::OnDirectoryOpen);
}

void ScreenshotMenu::OnMenuRequested(QPoint p_point)
{
  QPushButton *screenshotBtn = ThemeManager::get().GetWidgetType<ScreenshotButton>("screenshot");
  const QPoint l_global_point = screenshotBtn->mapToGlobal(p_point);
  popup(l_global_point);
}

void ScreenshotMenu::OnCaptureLog()
{
  audio::system::Play("screenshot");
  courtroom::viewport::screenshot(courtroom::Capture_ICLog);
}

void ScreenshotMenu::OnCaptureWindow()
{
  audio::system::Play("screenshot");
  courtroom::viewport::screenshot(courtroom::Capture_Window);
}

void ScreenshotMenu::OnDirectoryOpen()
{
  QUrl folderUrl = QUrl::fromLocalFile("base/screenshots");
  QDesktopServices::openUrl(folderUrl);
}
