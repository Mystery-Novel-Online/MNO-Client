#ifndef DEBUGWINDOW_H
#define DEBUGWINDOW_H
#include "FennEngine/engine/include/imgui/ImGuiWindow.h"

#include "NetworkWindow.h"
#include "ThemeWindow.h"

class DebugWindow : public ImGuiWindow {
public:
  DebugWindow();
  void draw() override;

private:
  void populatePlayerList();
  ThemeWindow m_windowTheme;
  NetworkWindow m_windowNetwork;
};


#endif // DEBUGWINDOW_H
