#include "user_metadata.h"
#include "modules/managers/character_manager.h"

CharacterId s_UserCharId = SpectatorId;
int s_clientId = 0;

QString s_showname = "";

namespace dro::network::metadata::user
{
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
    return IsSpectator() ? nullptr : CharacterManager::get().mServerCharacters.at(s_UserCharId).name;
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
      if(CharacterManager::get().p_SelectedCharacter != nullptr)
        return CharacterManager::get().p_SelectedCharacter->GetShowname().toStdString();
    }
    return s_showname.toStdString();
  }

  std::string getIniswap()
  {
    if(CharacterManager::get().p_SelectedCharacter != nullptr)
      return CharacterManager::get().p_SelectedCharacter->GetFolder().toStdString();
    return "Spectator";
  }

}
