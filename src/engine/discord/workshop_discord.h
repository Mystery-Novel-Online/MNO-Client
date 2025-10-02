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

  bool setRichPresenceState(bool state);
  WorkshopDiscord();
  void processOAuth();
  void runCallbacks();
  void sendUserMessage(std::string message, uint64_t recipientId);
  void updateRichPresence();

private:
  std::shared_ptr<discordpp::Client> m_currentClient = nullptr;
};

#endif // WORKSHOP_DISCORD_H
