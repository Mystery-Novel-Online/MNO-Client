#ifndef WORKSHOP_DISCORD_H
#define WORKSHOP_DISCORD_H

namespace discordpp
{
class Client;
class AuthorizationCodeVerifier;
}

class WorkshopDiscord : public QObject
{
  Q_OBJECT
public:
  static WorkshopDiscord& getInstance() {
    static WorkshopDiscord instance;
    return instance;
  }

  void sendPrivateMessage(const QString& discordId, const QString& message);
  void sendFriendRequest(const QString& discordId);

  void setRichPresenceState(bool state);
  void setRichPresenceStateText(std::string sstate);
  void setRichPresenceDetailsText(std::string sdetails);
  WorkshopDiscord();
  void processOAuth();
  void runCallbacks();
  void sendUserMessage(std::string message, uint64_t recipientId);
  void updateRichPresence();

  QVector<DiscordUser> getFriends();

private:
  std::shared_ptr<discordpp::Client> m_currentClient = nullptr;
};

#endif // WORKSHOP_DISCORD_H
