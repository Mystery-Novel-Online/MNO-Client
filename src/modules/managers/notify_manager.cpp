#include "notify_manager.h"
#include "pch.h"

#include "system/theme_scripting.h"

NotifyManager NotifyManager::s_Instance;

void NotifyManager::ThemeSetupPopup(ChoiceDialog *notify)
{
  u_choiceDialog = notify;
  HideNotification();
}

void NotifyManager::SetLuaNotification(QString dialog, QString eventName)
{
  mCurrentNotification = ChoiceEvent_Lua;
  SetRequestKey(eventName);
  SetText(dialog, true);
}

void NotifyManager::SetPairNotifcation()
{
  mCurrentNotification = ChoiceEvent_Pair;

  if(!LuaBridge::LuaEventCall("PairRequestEvent", mSenderName.toStdString()))
  {
    LuaBridge::LuaEventCall("OnPairRequest", mSenderName.toStdString());
    LuaFunctions::AlertUser(true);
    SetText(mSenderName + " has sent you a pair request.", true);
  }
}

void NotifyManager::ShowNotification()
{
  u_choiceDialog->setEventType(mCurrentNotification);
  u_choiceDialog->setKey(mRequestKey);
  u_choiceDialog->setSenderId(mSenderId);
  u_choiceDialog->show();
}

void NotifyManager::HideNotification()
{
  u_choiceDialog->hide();
}

void NotifyManager::SetSenderId(int id)
{
  mSenderId = id;
}

void NotifyManager::SetSenderName(QString sender)
{
  mSenderName = sender;
}

void NotifyManager::SetRequestKey(QString sender)
{
  mRequestKey = sender;
}

void NotifyManager::SetText(QString text, bool show)
{
  if(u_choiceDialog == nullptr) return;
  mCurrentNotificationMessage = text;
  u_choiceDialog->setText(text);
  if(show) ShowNotification();
}
