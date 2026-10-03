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

  WorkshopDiscord();

  void sendPrivateMessage(const QString& discordId, const QString& message);
  void sendFriendRequest(const QString& discordId);

  void setRichPresenceState(bool state);
  void setRichPresenceStateText(std::string sstate);
  void setRichPresenceDetailsText(std::string sdetails);
  void processOAuth();
  void runCallbacks();

  QVector<DiscordUser> getFriends();

private slots:
  void loginResult(bool staus, std::string token);

private:
  std::shared_ptr<discordpp::Client> m_currentClient = nullptr;
};

#endif // WORKSHOP_DISCORD_H
