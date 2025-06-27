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
  namespace partner
  {
    void setPartner(int clientId);
    bool isUnpaired();
  }

  bool IsSpectator();
  int GetCharacterId();
  bool SetCharacterId(int id);
  bool ChangeCharacterId(int id);
  QString GetCharacterName();

  void setShowname(QString showname);
  std::string getIniswap();
  std::string getShowname();

  int getClientId();
  int getOutgoingClientId();
  void setClientId(int id);
  void setIncomingId(int id);
}


#endif // USER_METADATA_H
