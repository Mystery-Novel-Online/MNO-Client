#include "user_metadata.h"
#include "modules/managers/character_manager.h"

CharacterId s_UserCharId = SpectatorId;

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

}
