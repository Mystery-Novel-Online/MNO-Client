#ifndef THEMEWINDOW_H
#define THEMEWINDOW_H
#include "FennEngine/engine/include/imgui/ImGuiWindow.h"

class ThemeWindow : public ImGuiWindow {
public:
  void draw() override;
};

#endif // THEMEWINDOW_H
