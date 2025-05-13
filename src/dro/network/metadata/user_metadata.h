#ifndef USER_METADATA_H
#define USER_METADATA_H

#include <QString>

using CharacterId = int;
enum : CharacterId
{
  SpectatorId = -1,
};

namespace dro::network::metadata::user
{
  bool IsSpectator();
  int GetCharacterId();
  bool SetCharacterId(int id);
  bool ChangeCharacterId(int id);
  QString GetCharacterName();
}


#endif // USER_METADATA_H
