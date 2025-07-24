#include "lobby_layout.h"
#include "pch.h"

#include "engine/system/theme.h"

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

  RPButton *CreateButton(QString name, QString image, std::function<void()> releasedFunction)
  {
    RPButton* rpButton = new RPButton(s_ConstructedLobby);
    rpButton->setName(name);
    rpButton->setScene(SceneType_ServerSelect);
    engine::system::theme::applyDimensions(rpButton, name, SceneType_ServerSelect);
    rpButton->set_image(image + ".png");

    QObject::connect(rpButton, &QPushButton::pressed, [rpButton, image]() { rpButton->set_image(image + "_pressed.png"); });

    QObject::connect(rpButton, &QPushButton::released, [rpButton, image, releasedFunction]()
    {
      rpButton->set_image(image + ".png");
      if (releasedFunction) {  releasedFunction(); }
    });

    return rpButton;
  }

}
