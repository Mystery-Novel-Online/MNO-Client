#include "choice_dialog.h"
#include "AOApplication.h"
#include "commondefs.h"
#include "drtheme.h"
#include "dro/fs/fs_reading.h"
#include "courtroom.h"
#include "modules/theme/thememanager.h"
#include "theme.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/theme_scripting.h"

ChoiceDialog::ChoiceDialog(QWidget *parent)
    : RPWidget{parent}
{
  m_choiceText = new DRTextEdit(this);
  m_acceptButton = new RPButton(this, m_App);
  m_declineButton = new RPButton(this, m_App);

  setDragable(true);

  ThemeReload();

  connect(m_acceptButton, &QAbstractButton::clicked, this, &ChoiceDialog::OnAcceptClicked);
  connect(m_declineButton, &QAbstractButton::clicked, this, &ChoiceDialog::OnDeclineClicked);
}

void ChoiceDialog::ThemeReload()
{
  resize(235, 135);
  setBackgroundImage("notify_bg");

  //Message Text
  m_choiceText->setFrameStyle(QFrame::NoFrame);
  ThemeManager::get().setWidgetPosition(m_choiceText, 5, 5);
  ThemeManager::get().setWidgetDimensions(m_choiceText, 224, 95);

  m_choiceText->setReadOnly(true);
  set_drtextedit_font(m_choiceText, "notify_popup", COURTROOM_FONTS_INI, m_App);
  m_choiceText->setPlainText(m_choiceText->toPlainText());

  //Accept Button
  SetupButton(m_acceptButton, 10, 100, 100, 30, "Accept");
  SetupButton(m_declineButton, 125, 100, 100, 30, "Decline");
}

void ChoiceDialog::SetupButton(RPButton *t_button, int t_x, int t_y, int t_width, int t_height, QString name)
{
  ThemeManager::get().setWidgetPosition(t_button, t_x, t_y);
  ThemeManager::get().setWidgetDimensions(t_button, t_width, t_height);

  QString l_WidgetName = "notify_" + name.toLower();

  t_button->set_image(m_App->current_theme->get_widget_image(l_WidgetName, l_WidgetName + ".png", "courtroom"));

  if (t_button->get_image().isEmpty()) t_button->setText(name);

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
    m_App->send_server_packet(DRPacket("PAIR", {QString::number(m_senderId), m_senderKey}));
    LuaBridge::LuaEventCall("OnPairRequestAccepted");
  }
  else if(m_choiceEvent == ChoiceEvent::ChoiceEvent_Lua)
  {
    QString luaEventName = m_senderKey + "Accepted";
    LuaBridge::LuaEventCall(luaEventName.toUtf8());
  }

  m_App->get_courtroom()->SetChatboxFocus();
  this->hide();
}

void ChoiceDialog::OnDeclineClicked()
{
  m_App->get_courtroom()->SetChatboxFocus();
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
