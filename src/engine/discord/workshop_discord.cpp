#include "workshop_discord.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <functional>
#include <csignal>

#define DISCORDPP_IMPLEMENTATION
#include <discord_social_sdk/discordpp.h>

#include <engine/param/json_reader.h>

#include <engine/network/api_manager.h>
std::shared_ptr<discordpp::Client> client;
const uint64_t APPLICATION_ID = 1409897527528128533;
std::atomic<bool> running = true;
bool rpEnabled = false;
QVector<DiscordUser> s_Friends = {};

discordpp::Activity currentActivity;

std::string state = "Lobby";
std::string details = "Selecting a server";

void refreshFriendsList()
{
  const auto onlineInGame = client->GetRelationships();
  s_Friends.clear();

  for (const auto& relationship : onlineInGame)
  {
    auto user = relationship.User();
    if (user) {
      if (relationship.GameRelationshipType() == discordpp::RelationshipType::Friend || relationship.DiscordRelationshipType() == discordpp::RelationshipType::Friend) {
        s_Friends.append({user->Id(), QString::fromStdString(user->Username()), QString::fromStdString(user->GlobalName().value_or(user->Username()))});
      }

    }
  }
}

void signalHandler(int signum) {
  running.store(false);
}

void tokenUpdated(discordpp::ClientResult result)
{
  if(result.Successful()) {
    std::cout << "🔑 Token updated, connecting to Discord...\n";
    client->Connect();
  }
}

void tokenRecieved(discordpp::ClientResult result, std::string accessToken, std::string refreshToken, discordpp::AuthorizationTokenType tokenType, int32_t expiresIn, std::string scope)
{
  std::cout << "🔓 Access token received! Establishing connection...\n";
  client->UpdateToken(discordpp::AuthorizationTokenType::Bearer,  accessToken, tokenUpdated);
}

void tokenRecieved(std::string accessToken)
{
  std::cout << "🔓 Access token received! Establishing connection...\n";
  client->UpdateToken(discordpp::AuthorizationTokenType::Bearer,  accessToken, tokenUpdated);
}

void richPresenceStatus(discordpp::ClientResult result)
{
  if(result.Successful()) {
    std::cout << "🎮 Rich Presence updated successfully!\n";
  } else {
    std::cerr << "❌ Rich Presence update failed";
  }
}

void setRichPresence()
{
  if(!rpEnabled)
  {
    client->ClearRichPresence();
    return;
  }

  currentActivity.SetType(discordpp::ActivityTypes::Playing);
  currentActivity.SetState(state);
  currentActivity.SetDetails(details);
  currentActivity.SetSupportedPlatforms(discordpp::ActivityGamePlatforms::Desktop);
  client->UpdateRichPresence(currentActivity, richPresenceStatus);
};

void clientStatusChangedCallback(discordpp::Client::Status status, discordpp::Client::Error error, int32_t errorDetail)
{
  std::cout << "[Client Status] Changed to " << discordpp::Client::StatusToString(status) << std::endl;

  switch (status)
  {
  case discordpp::Client::Status::Ready:
  {
    std::cout << "[!] Client is Ready!" << std::endl;

    discordpp::UserHandle me = client->GetCurrentUser();
    refreshFriendsList();


    std::cout << "[!] Logged in as " << me.Username() << " (ID: " << me.Id() << ")" << std::endl;
    std::cout << "[!] Friends count: " << client->GetRelationships().size() << std::endl;

    setRichPresence();
    break;
  }

  default:
    if (error != discordpp::Client::Error::None)
    {
      std::cerr << "[Client] Got Error: " << discordpp::Client::ErrorToString(error) << " (" << errorDetail << ")" << std::endl;
      running.store(false);
    }
    break;
  }
}

void authorizeClient()
{


}

void WorkshopDiscord::sendPrivateMessage(const QString &discordId, const QString &message)
{
  client->SendUserMessage(discordId.toLongLong(), message.toStdString(), [](auto result, uint64_t messageId) {
                            if (result.Successful()) {
                              std::cout << "✅ Message sent successfully\n";
                            } else {
                              std::cout << "❌ Failed to send message: " << result.Error() << "\n";
                            }
                          });
}

void WorkshopDiscord::sendFriendRequest(const QString &discordId)
{
  client->SendGameFriendRequestById(discordId.toLongLong(), [](discordpp::ClientResult result) {
                                      if (result.Successful()) {
                                        std::cout << "🎮 Game friend request sent successfully!\n";
                                      }
                                    });
}

void WorkshopDiscord::setRichPresenceState(bool state)
{
  rpEnabled = state;
  setRichPresence();
}

void WorkshopDiscord::setRichPresenceStateText(std::string sstate)
{
  state = sstate;
  setRichPresence();
}

void WorkshopDiscord::setRichPresenceDetailsText(std::string sdetails)
{
  details = sdetails;
  setRichPresence();
}

WorkshopDiscord::WorkshopDiscord()
{
  std::signal(SIGINT, signalHandler);

  m_currentClient = std::make_shared<discordpp::Client>();
  client = m_currentClient;
  client->SetStatusChangedCallback(clientStatusChangedCallback);
  m_currentClient->AddLogCallback([](auto message, auto severity) {
                                    std::cout << "[" << EnumToString(severity) << "] " << message << std::endl;
                                  }, discordpp::LoggingSeverity::Info);


  nlohmann::json verifyBody;
  verifyBody["user_key"] = ApiManager::authorizationKey().toStdString();

  QByteArray jsonData = QByteArray::fromStdString(verifyBody.dump());


  client->SetRelationshipGroupsUpdatedCallback([&](const uint64_t userId) { refreshFriendsList(); } );

  ApiManager::instance().login();
  connect(&ApiManager::instance(), &ApiManager::loginStatus, this, &WorkshopDiscord::loginResult);
}

void WorkshopDiscord::processOAuth()
{

  auto codeVerifier = m_currentClient->CreateAuthorizationCodeVerifier();

  discordpp::AuthorizationArgs args{};
  args.SetClientId(APPLICATION_ID);
  args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
  //args.SetScopes(discordpp::Client::GetDefaultCommunicationScopes());
  args.SetCodeChallenge(codeVerifier.Challenge());
  m_currentClient->Authorize(args, [this, codeVerifier](auto result, auto code, auto redirectUri) {
                               if (!result.Successful()) {
                                 std::cerr << "❌ Authentication Error: " << result.Error() << std::endl;
                                 return;
                               } else {
                                 std::cout << "✅ Authorization successful! Sending code to server...\n";

                                 nlohmann::json body;
                                 body["code"] = code;
                                 body["redirect_uri"] = redirectUri;
                                 body["code_verifier"] = codeVerifier.Verifier();

                                 QNetworkReply* reply = ApiManager::instance().post("api/users/discord/token", QByteArray::fromStdString(body.dump()));

                                 connect(reply, &QNetworkReply::finished, this, [reply, result]() {
                                           if (reply->error() != QNetworkReply::NoError) {
                                             std::cerr << "❌ Server request failed: " << reply->errorString().toStdString() << std::endl;
                                           } else {
                                             QString dataString = reply->readAll();
                                             std::cout << "✅ Token received from server: " << dataString.toStdString() << std::endl;
                                             JSONReader tokenData;
                                             tokenData.ReadFromString(dataString);
                                             config::ConfigUserSettings::setString("workshop_key", tokenData.getStringValue("workshop_key").toStdString());
                                             config::ConfigUserSettings::save();
                                             tokenRecieved(result, tokenData.getStringValue("access_token").toStdString(), tokenData.getStringValue("refresh_token").toStdString(), discordpp::AuthorizationTokenType::Bearer, tokenData.getIntValue("expires_in"),  tokenData.getStringValue("scope").toStdString());
                                           }
                                           reply->deleteLater();
                                         });
                               }
                             });
}



void WorkshopDiscord::runCallbacks()
{
  discordpp::RunCallbacks();
}

void userMessageStatus(discordpp::ClientResult result, uint64_t messageId)
{
  if (result.Successful()) {
    std::cout << "✅ Message sent successfully\n";
  } else {
    std::cout << "❌ Failed to send message: " << result.Error() << "\n";
  }
}

void WorkshopDiscord::sendUserMessage(std::string message, uint64_t recipientId)
{
  m_currentClient->SendUserMessage(recipientId, message, userMessageStatus);
}

void WorkshopDiscord::updateRichPresence()
{
  setRichPresence();
}

QVector<DiscordUser> WorkshopDiscord::getFriends()
{
  return s_Friends;
}

void WorkshopDiscord::loginResult(bool staus, std::string token)
{
  if (staus) {
    std::cout << "✅ Workshop key valid. Skipping OAuth and using server tokens.\n";
    tokenRecieved(token);
  } else {
    std::cout << "❌ Workshop key invalid or expired. Proceeding with OAuth authorization.\n";
    processOAuth();
  }
}

