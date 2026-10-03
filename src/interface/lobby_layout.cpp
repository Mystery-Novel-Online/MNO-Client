#include "lobby_layout.h"
#include "pch.h"

#include "system/theme.h"

#include "lobby.h"
#include "theme.h"

static Lobby* s_ConstructedLobby = nullptr;
static AOApplication* s_AOApplication = nullptr;

namespace Layout::ServerSelect
{
  void AssignLobby(Lobby *lobby, AOApplication *application)
  {
    s_ConstructedLobby = lobby;
    s_AOApplication = application;
  }
}
