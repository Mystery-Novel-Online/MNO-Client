#include "rpnotifymenu.h"
#include "AOApplication.h"
#include "commondefs.h"
#include "drtheme.h"
#include "file_functions.h"
#include "courtroom.h"
#include "modules/theme/thememanager.h"
#include "theme.h"

RPNotifyMenu::RPNotifyMenu(QWidget *parent)
    : QWidget{parent}
{
  AOApplication *ao_app = AOApplication::getInstance();

  pBackgroundImage = new DRStickerViewer(ao_app, this);
  pMessageText = new DRTextEdit(pBackgroundImage);
  pAcceptButton = new AOButton(pBackgroundImage, ao_app);
  pDeclineButton = new AOButton(pBackgroundImage, ao_app);

  ThemeReload();

  connect(pAcceptButton, &QAbstractButton::clicked, this, &RPNotifyMenu::NotifyAccept);
  connect(pDeclineButton, &QAbstractButton::clicked, this, &RPNotifyMenu::NotifyDecline);


}

void RPNotifyMenu::ThemeReload()
{

  AOApplication *lAOApp = AOApplication::getInstance();
  double themeResize = ThemeManager::get().getResize();

  //Background Image
  pBackgroundImage->move(0, 0);
  ThemeManager::get().setWidgetDimensions(pBackgroundImage, 235, 135);

  if (!lAOApp->find_theme_asset_path("notify_bg", animated_or_static_extensions()).isEmpty())
    pBackgroundImage->set_theme_image("notify_bg");

  set_sticker_play_once(pBackgroundImage, "notify_bg", COURTROOM_CONFIG_INI, lAOApp);

  //Message Text
  pMessageText->setFrameStyle(QFrame::NoFrame);
  ThemeManager::get().setWidgetPosition(pMessageText, 5, 5);
  ThemeManager::get().setWidgetDimensions(pMessageText, 224, 95);

  pMessageText->setReadOnly(true);
  set_drtextedit_font(pMessageText, "notify_popup", COURTROOM_FONTS_INI, lAOApp);
  pMessageText->setPlainText(pMessageText->toPlainText());

  //Accept Button
  SetupButton(pAcceptButton, 10, 100, 100, 30, "Accept");
  SetupButton(pDeclineButton, 125, 100, 100, 30, "Decline");
}

void RPNotifyMenu::SetupButton(AOButton *t_button, int t_x, int t_y, int t_width, int t_height, QString name)
{
  AOApplication *l_AOApp = AOApplication::getInstance();
  ThemeManager::get().setWidgetPosition(t_button, t_x, t_y);
  ThemeManager::get().setWidgetDimensions(t_button, t_width, t_height);

  QString l_WidgetName = "notify_" + name.toLower();

  t_button->set_image(l_AOApp->current_theme->get_widget_image(l_WidgetName, l_WidgetName + ".png", "courtroom"));

  if (t_button->get_image().isEmpty()) t_button->setText(name);

}

void RPNotifyMenu::SetNotificationText(QString message)
{
  pMessageText->setPlainText(message);
}

void RPNotifyMenu::SetNotificationType(NotificationType notify)
{
  mType = notify;
}

void RPNotifyMenu::SetNotificationKey(QString key)
{
  mKey = key;
}

void RPNotifyMenu::SetNotificationSender(int sender)
{
  mSender = sender;
}

void RPNotifyMenu::NotifyAccept()
{
  if(mType == NotificationType::PairRequest)
  {
    AOApplication::getInstance()->send_server_packet(DRPacket("PAIR", {QString::number(mSender), mKey}));
  }

  AOApplication::getInstance()->get_courtroom()->SetChatboxFocus();
  this->hide();
}

void RPNotifyMenu::NotifyDecline()
{
  AOApplication::getInstance()->get_courtroom()->SetChatboxFocus();
  this->hide();
}
