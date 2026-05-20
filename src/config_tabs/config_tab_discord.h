#ifndef CONFIG_TAB_DISCORD_H
#define CONFIG_TAB_DISCORD_H

#include <QWidget>

namespace Ui
{
class ConfigTabDiscord;
}

enum DiscordState
{
  PresenceStateServerSelect,
  PresenceStateConnecting,
  PresenceStateSpectating,
  PresenceStateCharacter,
  PresenceStateHidden
};

class ConfigTabDiscord : public QWidget
{
  Q_OBJECT

public:
  explicit ConfigTabDiscord(QWidget *parent = nullptr);
  ~ConfigTabDiscord();

  void toggleState(DiscordState state);
  void setServerText(const QString& text);
  void setCharacterText(const QString& text);
  void reloadState();

private slots:
  void on_discordPresence_toggled(bool arg1);

  void on_hideServer_stateChanged(int arg1);

  void on_hideCharacter_stateChanged(int arg1);

  QString serverText();
  QString characterText();

private:
  Ui::ConfigTabDiscord *ui;

  bool m_discordActive = false;
  bool m_hideCharacter = false;
  bool m_hideServer = false;

  DiscordState m_currentState = PresenceStateServerSelect;
  QString m_stateText = "";
  QString m_serverText = "";
};

#endif // CONFIG_TAB_DISCORD_H
