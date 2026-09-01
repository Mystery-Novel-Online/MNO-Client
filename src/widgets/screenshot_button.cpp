#include "screenshot_button.h"
#include "modules/theme/legacythememanager.h"
#include "interface/courtroom_layout.h"
#include "system/audio.h"
#include "interface/menus/screenshot_menu.h"

ScreenshotButton::ScreenshotButton(QWidget *parent, AOApplication *p_ao_app) : RPButton(parent)
{
  ScreenshotMenu* menu = new ScreenshotMenu();
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QWidget::customContextMenuRequested, menu, &ScreenshotMenu::OnMenuRequested);
  connect(this, &QAbstractButton::clicked, this, &ScreenshotButton::OnButtonClicked);
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
