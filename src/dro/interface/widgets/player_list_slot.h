#ifndef PLAYER_LIST_SLOT_H
#define PLAYER_LIST_SLOT_H
class AOApplication;

#include "dro/interface/widgets/image_display.h"
#include "dro/interface/widgets/rp_label.h"

#include <QWidget>

class DrPlayerListEntry : public QWidget
{
public:
    DrPlayerListEntry(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y);
    void set_character(QString p_character);
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
    RPLabel *m_prompt = nullptr;

    AOImageDisplay *pStatusDisplay = nullptr;

    int m_entrywidth = 0;

    void setQUrl(const QUrl &newQUrl);

  private slots:
  void openCharacterFolder();
  void openBrowserURL();
  void sendPairRequest();
  void sendUnpairRequest();
  void copyID();
  void copyHDID();
  void copyIPID();

private:
  AOApplication *ao_app = nullptr;

  int m_index = 0;
  QImage m_texture;
  QString m_showname;
  QString m_character;
  QString m_CharacterOutfit = "";
  QString mStatus = "";
  QString mURL = "";
  QString mHDID = "";
  QString mIPID = "";
  int mID = -1;

  void showContextMenu(QPoint pos);

};

#endif // PLAYER_LIST_SLOT_H
