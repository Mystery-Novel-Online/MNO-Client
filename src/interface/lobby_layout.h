#ifndef LOBBY_LAYOUT_H
#define LOBBY_LAYOUT_H

class RPButton;
class AOApplication;
class Lobby;

namespace Layout::ServerSelect
{
  void AssignLobby(Lobby *lobby, AOApplication* application);
}

#endif // LOBBY_LAYOUT_H
