#include "notify_manager.h"
#include "dro/system/theme_scripting.h"

NotifyManager NotifyManager::s_Instance;



void NotifyManager::ThemeSetupPopup(RPNotifyMenu *notify)
{
  pNotificationPopup = notify;
  HideNotification();
}

void NotifyManager::SetLuaNotification(QString dialog, QString eventName)
{
  mCurrentNotification = LuaEvent;
  SetRequestKey(eventName);
  SetText(dialog, true);
}

void NotifyManager::SetPairNotifcation()
{
  mCurrentNotification = PairRequest;

  if(!LuaBridge::LuaEventCall("PairRequestEvent", mSenderName.toStdString()))
  {
    LuaBridge::LuaEventCall("OnPairRequest", mSenderName.toStdString());
    LuaFunctions::AlertUser(true);
    SetText(mSenderName + " has sent you a pair request.", true);
  }
}

void NotifyManager::ShowNotification()
{
  pNotificationPopup->SetNotificationType(mCurrentNotification);
  pNotificationPopup->SetNotificationKey(mRequestKey);
  pNotificationPopup->SetNotificationSender(mSenderId);
  pNotificationPopup->show();
}

void NotifyManager::HideNotification()
{
  pNotificationPopup->hide();
}

void NotifyManager::ReloadNotification()
{
  if(pNotificationPopup != nullptr)
  {
    pNotificationPopup->ThemeReload();
  }
}

void NotifyManager::SetSenderId(int id)
{
  mSenderId = id;
}

void NotifyManager::SetSenderName(QString sender)
{
  mSenderName = sender;
}

void NotifyManager::SetSenderCharacter(QString sender)
{
  mSenderCharacter = sender;
}

void NotifyManager::SetRequestKey(QString sender)
{
  mRequestKey = sender;
}

void NotifyManager::SetText(QString text, bool show)
{
  if(pNotificationPopup == nullptr) return;
  mCurrentNotificationMessage = text;
  pNotificationPopup->SetNotificationText(text);
  if(show) ShowNotification();
}
