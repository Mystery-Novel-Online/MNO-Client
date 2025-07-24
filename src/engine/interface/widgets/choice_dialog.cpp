#include "choice_dialog.h"

#include "theme.h"
#include "engine/system/theme_scripting.h"
#include "modules/theme/thememanager.h"

ChoiceDialog::ChoiceDialog(QWidget *parent)
    : RPWidget{"notify_popup", parent}
{
  m_choiceText = new RPTextEdit("notify_text", this);
  m_acceptButton = new RPButton("notify_accept", "notify_accept.png", this);
  m_declineButton = new RPButton("notify_decline", "notify_decline.png",  this);

  setDragable(true);

  resetTransform();

  connect(m_acceptButton, &QAbstractButton::clicked, this, &ChoiceDialog::OnAcceptClicked);
  connect(m_declineButton, &QAbstractButton::clicked, this, &ChoiceDialog::OnDeclineClicked);
}

void ChoiceDialog::resetTransform()
{
  RPWidget::resetTransform();
  setBackgroundImage("notify_bg");

  m_choiceText->setFrameStyle(QFrame::NoFrame);
  m_choiceText->resetTransform();

  if(m_choiceText->width() == 0)
  {
    ThemeManager::get().setWidgetPosition(m_choiceText, 5, 5);
    ThemeManager::get().setWidgetDimensions(m_choiceText, 224, 95);
  }

  m_choiceText->setReadOnly(true);
  set_drtextedit_font(m_choiceText, "notify_popup", COURTROOM_FONTS_INI, m_app);
  m_choiceText->setPlainText(m_choiceText->toPlainText());

  m_acceptButton->refresh_position();
  m_acceptButton->refresh_image();
  m_declineButton->refresh_position();
  m_declineButton->refresh_image();

  if(m_acceptButton->width() == 0)
    SetupButton(m_acceptButton, 10, 100, 100, 30, "Accept");

  if(m_declineButton->width() == 0)
    SetupButton(m_declineButton, 125, 100, 100, 30, "Decline");
}

void ChoiceDialog::SetupButton(RPButton *t_button, int t_x, int t_y, int t_width, int t_height, QString name)
{
  ThemeManager::get().setWidgetPosition(t_button, t_x, t_y);
  ThemeManager::get().setWidgetDimensions(t_button, t_width, t_height);
}

void ChoiceDialog::setText(QString message)
{
  m_choiceText->setPlainText(message);
}

void ChoiceDialog::setEventType(ChoiceEvent notify)
{
  m_choiceEvent = notify;
}

void ChoiceDialog::setKey(QString key)
{
  m_senderKey = key;
}

void ChoiceDialog::setSenderId(int sender)
{
  m_senderId = sender;
}

void ChoiceDialog::OnAcceptClicked()
{
  if(m_choiceEvent == ChoiceEvent::ChoiceEvent_Pair)
  {
    m_app->send_server_packet(DRPacket("PAIR", {QString::number(m_senderId), m_senderKey}));
    LuaBridge::LuaEventCall("OnPairRequestAccepted");
  }
  else if(m_choiceEvent == ChoiceEvent::ChoiceEvent_Lua)
  {
    QString luaEventName = m_senderKey + "Accepted";
    LuaBridge::LuaEventCall(luaEventName.toUtf8());
  }

  m_app->get_courtroom()->SetChatboxFocus();
  this->hide();
}

void ChoiceDialog::OnDeclineClicked()
{
  m_app->get_courtroom()->SetChatboxFocus();
  this->hide();

  if(m_choiceEvent == ChoiceEvent::ChoiceEvent_Pair)
  {
    LuaBridge::LuaEventCall("OnPairRequestDeclined");
  }
  else if(m_choiceEvent == ChoiceEvent::ChoiceEvent_Lua)
  {
    QString luaEventName = m_senderKey + "Declined";
    LuaBridge::LuaEventCall(luaEventName.toUtf8());
  }


}
