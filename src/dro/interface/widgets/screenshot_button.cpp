#include "screenshot_button.h"
#include "modules/theme/thememanager.h"
#include "dro/interface/courtroom_layout.h"
#include "dro/system/audio.h"

ScreenshotButton::ScreenshotButton(QWidget *parent, AOApplication *p_ao_app) : RPButton(parent)
{
  connect(this, SIGNAL(clicked()), this, SLOT(OnButtonClicked()));
  UpdateDimensions();
  show();
}

void ScreenshotButton::UpdateDimensions()
{
  ThemeManager::get().AssignDimensions(this, "screenshot", RPSceneType::COURTROOM);
  set_theme_image("screenshot", "screenshot.png", "courtroom", "Screenshot");
}

void ScreenshotButton::OnButtonClicked()
{
  audio::system::Play("screenshot");
  courtroom::viewport::screenshot();
}
