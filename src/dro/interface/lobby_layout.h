#ifndef LOBBY_LAYOUT_H
#define LOBBY_LAYOUT_H

#include <QString>
#include <functional>

class AOButton;
class AOApplication;
class Lobby;

namespace Layout::ServerSelect
{
  void AssignLobby(Lobby *lobby, AOApplication* application);
  AOButton *CreateButton(QString name, QString image, std::function<void()> releasedFunction = nullptr);
}

#endif // LOBBY_LAYOUT_H
