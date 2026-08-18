#include "DebugWindow.h"

static bool WINDOW_VISIBLE_NETWORK = false;
static bool WINDOW_VISIBLE_THEME = false;
static bool WINDOW_VISIBLE_DOWNLOAD = false;

static int DEBUG_SIMULATED_PLAYER_COUNT = 12;

DebugWindow::DebugWindow()
{

}

void DebugWindow::draw()
{

  ImGui::Begin("Debug Tools");

  if(ImGui::Button("Populate Player List"))
  {
    populatePlayerList();
  }

  ImGui::Checkbox("Theme", &WINDOW_VISIBLE_THEME);
  ImGui::SameLine();
  ImGui::Checkbox("Network", &WINDOW_VISIBLE_NETWORK);
  ImGui::Checkbox("Downloads", &WINDOW_VISIBLE_DOWNLOAD);

  ImGui::End();

  if(WINDOW_VISIBLE_NETWORK)
    m_windowNetwork.draw();

  if(WINDOW_VISIBLE_THEME)
    m_windowTheme.draw();

  if(WINDOW_VISIBLE_DOWNLOAD)
    m_downloadWindow.draw();
}

void DebugWindow::populatePlayerList()
{
  SceneManager::get().clearPlayerDataList();
  for(int i = 0; i < DEBUG_SIMULATED_PLAYER_COUNT; i++)
  {
    DrPlayer drp(i, "Sim Player " + QString::number(i), "Persona" + QString::number(i), "", "", "Default");
    drp.data.showname = "Sim Player " + QString::number(i);
    drp.data.character = "Persona" + QString::number(i + 1);
    drp.data.afk = i % 3 == 1;
    drp.data.contentVersion = -1;
    SceneManager::get().mPlayerDataList.append(drp);
  }
  if(AOApplication::getInstance()->m_courtroom != nullptr)
    AOApplication::getInstance()->m_courtroom->construct_playerlist_layout();
}
