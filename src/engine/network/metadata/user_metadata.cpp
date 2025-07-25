#include "user_metadata.h"

#include "engine/param/actor_repository.h"
#include "engine/network/metadata/server_metadata.h"
#include "engine/param/actor/actor_loader.h"
CharacterId s_UserCharId = SpectatorId;
int s_clientId = 0;
int s_incomingId = 0;
static int s_partnerClientId = -1;


QString s_showname = "";

namespace engine::network::metadata::user
{
  namespace partner
  {

    void setPartner(int clientId)
    {
      s_partnerClientId = clientId;
    }

    bool isUnpaired()
    {
      return s_partnerClientId == -1;
    }

    int clientId()
    {
      return s_partnerClientId;
    }

  }

  bool IsSpectator()
  {
    return s_UserCharId == SpectatorId;
  }

  int GetCharacterId()
  {
    return s_UserCharId;
  }

  bool SetCharacterId(int id)
  {
    if (id != SpectatorId && s_UserCharId == id) return false;
    s_UserCharId = id;
    return true;
  }

  QString GetCharacterName()
  {
    return IsSpectator() ? nullptr : CharacterRepository::characterNameServer(s_UserCharId);
  }

  bool ChangeCharacterId(int id)
  {
    const bool charaChanged = (s_UserCharId != id);
    s_UserCharId = id;
    return charaChanged;
  }

  int getClientId()
  {
    return s_clientId;
  }

  void setClientId(int id)
  {
    s_clientId = id;
  }

  void setShowname(QString showname)
  {
    s_showname = showname;
  }

  std::string getShowname()
  {
    if(s_showname.trimmed().isEmpty())
    {
      if(engine::actor::user::retrieve() != nullptr)
        return engine::actor::user::retrieve()->showname();
    }
    return s_showname.toStdString();
  }

  std::string getIniswap()
  {
    if(engine::actor::user::retrieve() != nullptr)
      return engine::actor::user::retrieve()->folder();
    return "Spectator";
  }

  void setIncomingId(int id)
  {
    s_incomingId = id;
  }

  int getOutgoingClientId()
  {
    const QString baseVersionString = QString::number(s_incomingId);
    if(baseVersionString.length() >= 6 && !baseVersionString.startsWith("-"))
    {
      return s_clientId + 1 + QString(baseVersionString.at(2)).toInt();
    }
    else
      return s_clientId;

  }

}
