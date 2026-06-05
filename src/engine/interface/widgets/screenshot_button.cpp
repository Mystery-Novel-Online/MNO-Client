#include "screenshot_button.h"
#include "modules/theme/legacythememanager.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/system/audio.h"
#include "engine/interface/menus/screenshot_menu.h"

ScreenshotButton::ScreenshotButton(QWidget *parent, AOApplication *p_ao_app) : RPButton(parent)
{
  ScreenshotMenu* menu = new ScreenshotMenu();
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QWidget::customContextMenuRequested, menu, &ScreenshotMenu::OnMenuRequested);
  connect(this, SIGNAL(clicked()), this, SLOT(OnButtonClicked()));
  UpdateDimensions();
  show();
}

void ScreenshotButton::UpdateDimensions()
{
  LegacyThemeManager::get().AssignDimensions(this, "screenshot", ThemeSceneType::SceneType_Courtroom);
  set_theme_image("screenshot", "screenshot.png", "courtroom", "Screenshot");
}

void ScreenshotButton::OnButtonClicked()
{
  audio::system::Play("screenshot");
  courtroom::viewport::screenshot();
}
