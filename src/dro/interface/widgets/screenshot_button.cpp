#include "screenshot_button.h"
#include "modules/theme/thememanager.h"
#include "modules/managers/scene_manager.h"

#include "dro/system/audio.h"

ScreenshotButton::ScreenshotButton(QWidget *parent, AOApplication *p_ao_app) : RPButton(parent, p_ao_app)
{
  connect(this, SIGNAL(clicked()), this, SLOT(OnButtonClicked()));
  UpdateDimensions();
  show();
}

void ScreenshotButton::UpdateDimensions()
{
  ThemeManager::get().AssignDimensions(this, "screenshot", ThemeSceneType::COURTROOM);
  set_theme_image("screenshot", "screenshot.png", "courtroom", "Screenshot");
}

void ScreenshotButton::OnButtonClicked()
{
  audio::effect::Play("screenshot");
  SceneManager::get().ScreenshotViewport();
}
