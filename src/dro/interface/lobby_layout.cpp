#include "lobby_layout.h"
#include "dro/interface/widgets/rp_button.h"
#include "lobby.h"
#include "aoapplication.h"
#include "theme.h"
#include "commondefs.h"
#include "dro/system/theme.h"

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
    RPButton* newButton = new RPButton(s_ConstructedLobby);
    newButton->setName(name);
    newButton->setScene(SceneType_ServerSelect);
    dro::system::theme::applyDimensions(newButton, name, SceneType_ServerSelect);
    newButton->set_image(image + ".png");

    QObject::connect(newButton, &QPushButton::pressed, [newButton, image]() { newButton->set_image(image + "_pressed.png"); });

    QObject::connect(newButton, &QPushButton::released, [newButton, image, releasedFunction]()
    {
      newButton->set_image(image + ".png");
      if (releasedFunction) {  releasedFunction(); }
    });

    return newButton;
  }

}
