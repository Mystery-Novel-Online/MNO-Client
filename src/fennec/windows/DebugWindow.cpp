#include "DebugWindow.h"

static bool WINDOW_VISIBLE_NETWORK = false;
static bool WINDOW_VISIBLE_THEME = false;

DebugWindow::DebugWindow()
{

}

void DebugWindow::draw()
{

  ImGui::Begin("Debug Tools");

  ImGui::Checkbox("Theme", &WINDOW_VISIBLE_THEME);
  ImGui::SameLine();
  ImGui::Checkbox("Network", &WINDOW_VISIBLE_NETWORK);

  ImGui::End();

  if(WINDOW_VISIBLE_NETWORK)
    m_windowNetwork.draw();

  if(WINDOW_VISIBLE_THEME)
    m_windowTheme.draw();
}
