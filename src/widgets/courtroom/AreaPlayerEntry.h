#ifndef PLAYER_LIST_SLOT_H
#define PLAYER_LIST_SLOT_H
class AOApplication;

#include <QWidget>

#include <mk2/drplayer.h>

class AreaPlayerEntry : public RPWidget
{
public:
  AreaPlayerEntry(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y, const DrPlayer& player);

  void themeReload();
  void updateData(const DrPlayer& player, int y);

  void refreshManual(int width);
  void refreshAutomatic(int width);

  void toggleTyping(bool status);

  void setCharacter(const QString& p_character, bool afkState);
  void setOutfit(const QString& outfitName);
  void setShowname(const QString& showname);
  void setReason(const QString& a_reasonText);
  void setStatus(const QString& a_statusText);
  void setContentVersion(int versionNumber);

  inline int clientId() const { return m_playerData.data.id; }

private slots:
  void addDiscordFriend();
  void messageDiscordFriend();

  void openCharacterFolder();
  void openBrowserURL();
  void sendPairRequest();
  void sendUnpairRequest();
  void sendLayerFront();
  void sendLayerBack();
  void copyID();
  void copyHDID();
  void copyIPID();
  void followPlayer();
  void handleTypingTimeout();

private:
  AOApplication *ao_app = nullptr;

  QString m_showname;
  QString m_playerCharacter;
  QString m_playerOutfit;
  DrPlayer m_playerData;

  bool m_afk = false;
  bool m_missingPlayerIcon = false;

  void showContextMenu(QPoint pos);

  QTimer* m_typingTimer = nullptr;

  RPLabel *u_shownameDisplay = nullptr;
  RPLabel *u_typingIndicator = nullptr;
  RPLabel *u_areaPrompt = nullptr;

  AOImageDisplay *u_playerIconBorder = nullptr;
  AOImageDisplay *u_playerIcon = nullptr;
  AOImageDisplay *u_statusDisplay = nullptr;
  AOImageDisplay *u_updateDisplay = nullptr;

  QImage m_texture;

};

#endif // PLAYER_LIST_SLOT_H
