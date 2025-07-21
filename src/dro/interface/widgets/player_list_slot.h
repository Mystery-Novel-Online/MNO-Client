#ifndef PLAYER_LIST_SLOT_H
#define PLAYER_LIST_SLOT_H
class AOApplication;

#include <QWidget>

class DrPlayerListEntry : public RPWidget
{
public:
  DrPlayerListEntry(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y);

  void refreshManual(int width);
  void refreshAutomatic(int width);

  void toggleTyping(bool status);

  void set_character(QString p_character, bool afkState);
  void setOutfit(QString outfitName);
  void set_name(QString showname);
  void set_reason(QString p_reason);
  void setURL(QString url);
  void setID(int id);
  void setStatus(QString status);
  void setMod(QString ipid, QString hdid);
  AOImageDisplay *pCharacterBorderDisplay = nullptr;
  AOImageDisplay *ui_user_image = nullptr;
  RPLabel *ui_showname = nullptr;
  RPLabel *ui_typing = nullptr;
  RPLabel *m_prompt = nullptr;

  AOImageDisplay *pStatusDisplay = nullptr;

  void setQUrl(const QUrl &newQUrl);

public:
  int clientId();


private slots:
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
  QImage m_texture;
  QString m_showname;
  QString m_character;
  bool m_afk = false;
  QString m_CharacterOutfit = "";
  QString mStatus = "";
  QString mURL = "";
  QString mHDID = "";
  QString mIPID = "";
  int m_clientId = -1;

  QTimer* m_typingTimer = nullptr;

  void showContextMenu(QPoint pos);

};

#endif // PLAYER_LIST_SLOT_H
