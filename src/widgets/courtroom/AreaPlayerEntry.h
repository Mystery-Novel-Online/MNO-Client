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

  void set_character(QString p_character, bool afkState);
  void setOutfit(QString outfitName);
  void set_name(QString showname);
  void set_reason(QString p_reason);
  void setID(int id);
  void setStatus(QString status);
  void setContentVersion(int versionNumber);
  AOImageDisplay *pCharacterBorderDisplay = nullptr;
  AOImageDisplay *ui_user_image = nullptr;
  RPLabel *ui_showname = nullptr;
  RPLabel *ui_typing = nullptr;
  RPLabel *m_prompt = nullptr;

  AOImageDisplay *pStatusDisplay = nullptr;
  AOImageDisplay *w_UpdateDisplay = nullptr;

  void setQUrl(const QUrl &newQUrl);

public:
  int clientId();


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

  int m_index = 0;
  QString m_showname;
  QString m_character;
  QString m_CharacterOutfit = "";

  QImage m_texture;

  bool m_afk = false;
  QString mStatus = "";

  QTimer* m_typingTimer = nullptr;

  DrPlayer m_playerData;

  bool m_charIconMissing = false;

  void showContextMenu(QPoint pos);

};

#endif // PLAYER_LIST_SLOT_H
