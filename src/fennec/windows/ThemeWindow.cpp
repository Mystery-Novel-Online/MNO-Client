#include "ThemeWindow.h"

#include <modules/theme/legacythememanager.h>

void ThemeWindow::draw()
{
  ImGui::Begin("Theme Window");

  static QString selectedWidget;

  ImGui::BeginChild("Widget List", ImVec2(200, 0), true);

  for (auto it = LegacyThemeManager::get().widgetsList().begin(); it != LegacyThemeManager::get().widgetsList().end(); ++it)
  {
    const QString& name = it.key();

    if (ImGui::Selectable(name.toUtf8().constData(), selectedWidget == name))
    {
      selectedWidget = name;
    }
  }

  ImGui::EndChild();

  ImGui::SameLine();

  ImGui::BeginChild("Widget Inspector", ImVec2(0, 0), true);

  QWidget* widget = LegacyThemeManager::get().widgetsList().value(selectedWidget, nullptr);

  if (widget)
  {
    ImGui::Text("Widget: %s", selectedWidget.toUtf8().constData());

    ImGui::Separator();

    QSize size = widget->size();
    QPoint pos = widget->pos();
    bool visible = widget->isVisible();

    if (ImGui::Checkbox("Visible", &visible))
      widget->setVisible(visible);

    int width = size.width();
    int height = size.height();
    int x = pos.x();
    int y = pos.y();

    if (ImGui::SliderInt("Width", &width, 0, 2000))
      widget->resize(width, height);

    if (ImGui::SliderInt("Height", &height, 0, 2000))
      widget->resize(width, height);

    if (ImGui::SliderInt("X", &x, 0, 2000) || ImGui::SliderInt("Y", &y, 0, 2000))
    {
      widget->move(x, y);
    }

  }
  else
  {
    ImGui::Text("Select a widget");
  }

  ImGui::EndChild();

  ImGui::End();
}
