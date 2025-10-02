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
std::shared_ptr<discordpp::Client> client;
const uint64_t APPLICATION_ID = 1409897527528128533;
std::atomic<bool> running = true;
bool rpEnabled = false;

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
  discordpp::Activity activity;
  activity.SetType(discordpp::ActivityTypes::Playing);
  activity.SetState("Browsing Workshop");
  activity.SetDetails("Nothing important here");
  client->UpdateRichPresence(activity, richPresenceStatus);
  client->ClearRichPresence();
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

bool WorkshopDiscord::setRichPresenceState(bool state)
{

}

WorkshopDiscord::WorkshopDiscord()
{
  std::signal(SIGINT, signalHandler);
  std::cout << "🚀 Initializing Discord SDK...\n";

  m_currentClient = std::make_shared<discordpp::Client>();
  client = m_currentClient;
  client->SetStatusChangedCallback(clientStatusChangedCallback);
  m_currentClient->AddLogCallback([](auto message, auto severity) { std::cout << "[" << EnumToString(severity) << "] " << message << std::endl; }, discordpp::LoggingSeverity::Info);

    nlohmann::json verifyBody;

    verifyBody["user_key"] = config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE");

    QString uri = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/"));
    QNetworkRequest verifyRequest(QUrl(uri + "api/users/discord/verify"));
    verifyRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkAccessManager* verifyManager = new QNetworkAccessManager(this);
    QNetworkReply* verifyReply = verifyManager->post(verifyRequest, QByteArray::fromStdString(verifyBody.dump()));

    connect(verifyReply, &QNetworkReply::finished, this, [this, verifyReply]() {
              if (verifyReply->error() != QNetworkReply::NoError) {
                std::cerr << "❌ Verification request failed: " << verifyReply->errorString().toStdString() << std::endl;
                verifyReply->deleteLater();
                processOAuth();
                return;
              }

              QString dataString = verifyReply->readAll();
              nlohmann::json verifyResponse = nlohmann::json::parse(dataString.toStdString());
              verifyReply->deleteLater();

              bool isValid = verifyResponse.value("valid", false);

              if (isValid) {
                std::cout << "✅ Workshop key valid. Skipping OAuth and using server tokens.\n";
                tokenRecieved(verifyResponse["access_token"].get<std::string>());
              } else {
                std::cout << "❌ Workshop key invalid or expired. Proceeding with OAuth authorization.\n";
                processOAuth();
              }
            });

}

void WorkshopDiscord::processOAuth()
{

  auto codeVerifier = m_currentClient->CreateAuthorizationCodeVerifier();

  discordpp::AuthorizationArgs args{};
  args.SetClientId(APPLICATION_ID);
  args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
  args.SetScopes(discordpp::Client::GetDefaultCommunicationScopes());
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

                                 QString uri = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/"));
                                 QNetworkRequest request(QUrl(uri + "api/users/discord/token"));
                                 request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

                                 QNetworkAccessManager* manager = new QNetworkAccessManager(this);
                                 QNetworkReply* reply = manager->post(request, QByteArray::fromStdString(body.dump()));

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
