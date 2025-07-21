#ifndef NOTIFYMANAGER_H
#define NOTIFYMANAGER_H

class NotifyManager
{
public:
  NotifyManager(const NotifyManager&) = delete;

  static NotifyManager& get()
  {
    return s_Instance;
  }

  void ThemeSetupPopup(ChoiceDialog *notify);

  void SetLuaNotification(QString dialog, QString eventName);
  void SetPairNotifcation();
  void ShowNotification();
  void HideNotification();

  void SetSenderId(int id);
  void SetSenderName(QString sender);
  void SetRequestKey(QString sender);

  void SetText(QString text, bool show);


private:
  int mSenderId = -1;
  QString mRequestKey = "";
  QString mSenderName = "SYSTEM";
  QString mCurrentNotificationMessage = "";
  ChoiceEvent mCurrentNotification = ChoiceEvent_Invalid;

  NotifyManager() {}
  static NotifyManager s_Instance;

  ChoiceDialog *pNotificationPopup;

};

#endif // NOTIFYMANAGER_H
