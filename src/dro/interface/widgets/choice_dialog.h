#ifndef CHOICE_DIALOG_H
#define CHOICE_DIALOG_H

#include <QWidget>
#include "dro/interface/widgets/sticker_viewer.h"
#include "dro/interface/widgets/rp_text_edit.h"
#include "dro/interface/widgets/rp_widget.h"
#include "dro/interface/widgets/rp_button.h"

enum ChoiceEvent
{
  ChoiceEvent_Invalid = 0,
  ChoiceEvent_Pair = 1,
  ChoiceEvent_Party = 2,
  ChoiceEvent_Reserved = 3,
  ChoiceEvent_Lua = 4

};

class ChoiceDialog : public RPWidget
{
  Q_OBJECT
public:
  explicit ChoiceDialog(QWidget *parent = nullptr);
  void ThemeReload();
  void SetupButton(RPButton* t_button, int t_x, int t_y, int t_width, int t_height, QString name);

  void setText(QString message);

  void setSenderId(int sender);
  void setEventType(ChoiceEvent notify);
  void setKey(QString key);

private slots:
  void OnAcceptClicked();
  void OnDeclineClicked();

private:
  DRTextEdit *m_choiceText = nullptr;
  RPButton *m_acceptButton = nullptr;
  RPButton *m_declineButton = nullptr;

  QString m_senderKey = "";
  int m_senderId = -1;
  ChoiceEvent m_choiceEvent = ChoiceEvent_Invalid;

signals:

};

#endif // CHOICE_DIALOG_H
