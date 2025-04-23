#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

#include <QString>

#include <modules/widgets/rpnotifymenu.h>



class NotifyManager
{
public:
  NotifyManager(const NotifyManager&) = delete;

  static NotifyManager& get()
  {
    return s_Instance;
  }

  void ThemeSetupPopup(RPNotifyMenu *notify);

  void SetLuaNotification(QString dialog, QString eventName);
  void SetPairNotifcation();
  void ShowNotification();
  void HideNotification();
  void ReloadNotification();

  void SetSenderId(int id);
  void SetSenderName(QString sender);
  void SetSenderCharacter(QString sender);
  void SetRequestKey(QString sender);

  void SetText(QString text, bool show);


private:
  int mSenderId = -1;
  QString mRequestKey = "";
  QString mSenderCharacter = "SYSTEM";
  QString mSenderName = "SYSTEM";
  QString mCurrentNotificationMessage = "";
  NotificationType mCurrentNotification = INVALID;

  NotifyManager() {}
  static NotifyManager s_Instance;

  RPNotifyMenu *pNotificationPopup;

};

#endif // NOTIFYMANAGER_H
