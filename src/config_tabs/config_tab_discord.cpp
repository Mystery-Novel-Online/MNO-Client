#include "config_tab_discord.h"
#include "ui_config_tab_discord.h"

#include <engine/discord/workshop_discord.h>

ConfigTabDiscord::ConfigTabDiscord(QWidget *parent) : QWidget(parent), ui(new Ui::ConfigTabDiscord)
{
  m_discordActive = config::ConfigUserSettings::booleanValue("discord_rich_presence", true);
  m_hideCharacter = config::ConfigUserSettings::booleanValue("discord_hide_character", false);
  m_hideServer = config::ConfigUserSettings::booleanValue("discord_hide_server", false);

  ui->setupUi(this);

  ui->discordPresence->setChecked(m_discordActive);
  ui->hideCharacter->setChecked(m_hideCharacter);
  ui->hideServer->setChecked(m_hideServer);
}

ConfigTabDiscord::~ConfigTabDiscord()
{
  delete ui;
}

void ConfigTabDiscord::toggleState(DiscordState state)
{
  m_currentState = state;
  reloadState();
}

void ConfigTabDiscord::setServerText(const QString &text)
{
  m_serverText = text;
}

void ConfigTabDiscord::setCharacterText(const QString &text)
{
  m_stateText = text;
  m_currentState = PresenceStateCharacter;
  reloadState();
}

void ConfigTabDiscord::reloadState()
{
  QString details = "";
  QString presence = "";

  switch(m_currentState)
  {
    case PresenceStateCharacter:
    case PresenceStateSpectating:
      details = serverText();
      presence = characterText();
      break;

    case PresenceStateConnecting:
      details = serverText();
      presence = "Connecting...";
      break;

    case PresenceStateServerSelect:
      presence = "Selecting a server";
      details = "Lobby";
      break;

    default:
      break;
  }

  if(!details.isEmpty())
    WorkshopDiscord::getInstance().setRichPresenceDetailsText(details.toStdString());

  WorkshopDiscord::getInstance().setRichPresenceStateText(presence.toStdString());
}

void ConfigTabDiscord::on_discordPresence_toggled(bool arg1)
{
  m_discordActive = arg1;
  WorkshopDiscord::getInstance().setRichPresenceState(m_discordActive);
  config::ConfigUserSettings::setValue("discord_rich_presence", m_discordActive);
  reloadState();
}


void ConfigTabDiscord::on_hideServer_stateChanged(int arg1)
{
  m_hideServer = ui->hideServer->isChecked();
  config::ConfigUserSettings::setValue("discord_hide_server", m_hideServer);
  reloadState();
}


void ConfigTabDiscord::on_hideCharacter_stateChanged(int arg1)
{
  m_hideCharacter = ui->hideCharacter->isChecked();
  config::ConfigUserSettings::setValue("discord_hide_character", m_hideCharacter);
  reloadState();
}

QString ConfigTabDiscord::serverText()
{
  if(m_hideServer || m_serverText.isEmpty())
    return "In a server";

  return m_serverText;
}

QString ConfigTabDiscord::characterText()
{
  if(m_hideCharacter)
    return "Keeping their identity hidden.";

  if(m_currentState == PresenceStateSpectating)
    return "Spectating";

  return "Playing as " + m_stateText;
}

