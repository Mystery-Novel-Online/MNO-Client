#include "drplayerlistentry.h"
#include "aoapplication.h"
#include "commondefs.h"
#include "file_functions.h"
#include "modules/theme/thememanager.h"
#include "theme.h"
#include "file_functions.h"
#include "modules/managers/pair_manager.h"
#include "modules/managers/localization_manager.h"

#include <QMenu>
#include <QUrl>
#include <QDesktopServices>
#include <QClipboard>

DrPlayerListEntry::DrPlayerListEntry(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y)
    : QWidget(p_parent)
{

  double themeResize = ThemeManager::get().getResize();
  int resize_height = (int)((float)50 * ThemeManager::get().getResize());


  int statusResize = (int)((float)26 * ThemeManager::get().getResize());

    ao_app = p_ao_app;
    m_entrywidth = p_parent->size().width();
    this->resize(m_entrywidth, resize_height);

    this->move(p_x, p_y);

    ui_showname = new AOLabel(this, ao_app);
    ui_showname->move(resize_height, 7);
    ui_showname->resize(m_entrywidth-resize_height, 16);
    set_stylesheet(ui_showname, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

    ui_user_image = new AOImageDisplay(this, ao_app);
    ui_user_image->move((int)((float)5 * themeResize), (int)((float)5 * themeResize));
    ui_user_image->resize((int)((float)40 * ThemeManager::get().getResize()), (int)((float)40 * ThemeManager::get().getResize()));


    pCharacterBorderDisplay = new AOImageDisplay(this, ao_app);
    pCharacterBorderDisplay->move(0, 0);
    pCharacterBorderDisplay->resize(resize_height, resize_height);

    pStatusDisplay = new AOImageDisplay(this, ao_app);
    pStatusDisplay->move((int)((float)30 * themeResize),(int)((float)23 * themeResize));
    pStatusDisplay->resize(statusResize, statusResize);

    const QString lStatusImagePath = ao_app->find_theme_asset_path("player_list_status.png");

    if (file_exists(lStatusImagePath)) pStatusDisplay->set_image(lStatusImagePath);


    const QString l_selected_texture = ao_app->find_theme_asset_path("char_border.png");

    if (file_exists(l_selected_texture)) pCharacterBorderDisplay->set_image(l_selected_texture);

    //Prompt (For Blackouts / Look)
    m_prompt = new AOLabel(this, ao_app);
    m_prompt->move(5, 5);
    m_prompt->resize(m_entrywidth, resize_height);
    m_prompt->setWordWrap(true);
    set_stylesheet(m_prompt, "[PLAYER LIST PROMPT]", COURTROOM_STYLESHEETS_CSS, ao_app);

    ui_showname->hide();
    ui_user_image->hide();
    pCharacterBorderDisplay->hide();
    pStatusDisplay->hide();
    m_prompt->hide();

    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this ,&QWidget::customContextMenuRequested, this, &DrPlayerListEntry::showContextMenu);

}


void DrPlayerListEntry::set_character(QString p_character)
{

  m_character = p_character;
  const QString l_icon_path = ao_app->get_character_path(m_character, "char_icon.png");

  const bool l_file_exist = file_exists(l_icon_path);
  if(l_file_exist)
  {
      ui_user_image->set_image(l_icon_path);

      const QString l_selected_texture = ao_app->get_character_path(p_character, "char_border.png");

      if (file_exists(l_selected_texture)) pCharacterBorderDisplay->set_image(l_selected_texture);

  }
  else
  {
      QString l_missing_char_image = ao_app->find_theme_asset_path("missing_char.png");
      if (!l_missing_char_image.isEmpty())
      {
          ui_user_image->set_theme_image("missing_char.png");
      }

  }
  ui_user_image->show();
  pCharacterBorderDisplay->show();

  m_prompt->hide();

}

void DrPlayerListEntry::set_name(QString showname)
{
  ui_showname->show();
  m_prompt->hide();
  m_showname = showname;
  ui_showname->setText(showname);

}

void DrPlayerListEntry::set_reason(QString p_reason)
{
  ui_showname->hide();
  ui_user_image->hide();
  pCharacterBorderDisplay->hide();
  m_prompt->show();

  m_prompt->setText(p_reason);
}

void DrPlayerListEntry::setURL(QString url)
{
  mURL = url;
}

void DrPlayerListEntry::setID(int id)
{
  mID = id;
}

void DrPlayerListEntry::setStatus(QString status)
{
  if(!status.isEmpty())
  {
      mStatus = status;
      setToolTip(status);
      pStatusDisplay->show();
  }
}

void DrPlayerListEntry::setMod(QString ipid, QString hdid)
{
  mIPID = ipid;
  mHDID = hdid;
}

void DrPlayerListEntry::openCharacterFolder()
{

  QString folderPath = "characters/" + m_character;

  QUrl folderUrl = QUrl::fromLocalFile(ao_app->get_package_or_base_path(folderPath));

  QDesktopServices::openUrl(folderUrl);
}

void DrPlayerListEntry::openBrowserURL()
{
  if(!mURL.isEmpty())
  {
      QUrl url(mURL);
      QDesktopServices::openUrl(url);

  }
}

void DrPlayerListEntry::sendPairRequest()
{
  ao_app->send_server_packet(DRPacket("PR", {QString::number(mID)}));
}

void DrPlayerListEntry::sendUnpairRequest()
{
  ao_app->send_server_packet(DRPacket("UPR", {QString::number(mID)}));
}

void DrPlayerListEntry::copyID()
{
  QClipboard *clipboard = QGuiApplication::clipboard();

  clipboard->setText(QString::number(mID));
}

void DrPlayerListEntry::copyHDID()
{
  QClipboard *clipboard = QGuiApplication::clipboard();

  clipboard->setText(mHDID);
}

void DrPlayerListEntry::copyIPID()
{
  QClipboard *clipboard = QGuiApplication::clipboard();

  clipboard->setText(mIPID);
}

void DrPlayerListEntry::showContextMenu(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->addAction("[" + QString::number(mID) + "] " + m_showname);

  if(PairManager::get().GetCanPair())
  {
      QAction *pairRequest = new QAction(LocalizationManager::get().getLocalizationText("PLAYER_LIST_PAIR"));
      QObject::connect(pairRequest, &QAction::triggered, [this](){sendPairRequest() ;});
      menu->addAction(pairRequest);
  }
  else
  {
      QAction *pairRequest = new QAction(LocalizationManager::get().getLocalizationText("PLAYER_LIST_UNPAIR"));
      QObject::connect(pairRequest, &QAction::triggered, [this](){sendUnpairRequest() ;});
      menu->addAction(pairRequest);
  }


  QAction *a = new QAction(LocalizationManager::get().getLocalizationText("OPEN_CHAR_FOLDER"));
  QObject::connect(a, &QAction::triggered, [this](){openCharacterFolder();});
  menu->addAction(a);

  if(!mURL.isEmpty())
  {
      QUrl url(mURL);
      QAction *browserOpen = new QAction("Open " + url.host() + " in Browser");
      QObject::connect(browserOpen, &QAction::triggered, [this](){openBrowserURL();});
      menu->addAction(browserOpen);
  }

  if(!mHDID.isEmpty())
  {
      QAction *copyHDIDaction = new QAction(LocalizationManager::get().getLocalizationText("MOD_COPY_HDID") + " [" + mHDID + "]");
      QObject::connect(copyHDIDaction, &QAction::triggered, [this](){copyHDID();});
      menu->addAction(copyHDIDaction);
  }

  if(!mIPID.isEmpty())
  {
      QAction *copyHDIDaction = new QAction(LocalizationManager::get().getLocalizationText("MOD_COPY_IPID") + " [" + mIPID + "]");
      QObject::connect(copyHDIDaction, &QAction::triggered, [this](){copyIPID();});
      menu->addAction(copyHDIDaction);
  }

  QAction *copyIDAction = new QAction(LocalizationManager::get().getLocalizationText("PLAYER_LIST_ID"));
  QObject::connect(copyIDAction, &QAction::triggered, [this](){copyID();});
  menu->addAction(copyIDAction);

  menu->popup(this->mapToGlobal(pos));
}
