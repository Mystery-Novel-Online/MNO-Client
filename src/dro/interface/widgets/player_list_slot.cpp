#include "player_list_slot.h"
#include "aoapplication.h"
#include "commondefs.h"
#include "dro/fs/fs_reading.h"
#include "modules/theme/thememanager.h"
#include "theme.h"
#include "dro/fs/fs_reading.h"
#include "modules/managers/pair_manager.h"
#include "dro/system/localization.h"
#include "dro/network/metadata/user_metadata.h"

#include <QMenu>
#include <QUrl>
#include <QDesktopServices>
#include <QClipboard>

using namespace dro::network::metadata;
using namespace dro::system;

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

    ui_showname = new RPLabel(this, ao_app);
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

    if (FS::Checks::FileExists(lStatusImagePath)) pStatusDisplay->set_image(lStatusImagePath);


    const QString l_selected_texture = ao_app->find_theme_asset_path("char_border.png");

    if (FS::Checks::FileExists(l_selected_texture)) pCharacterBorderDisplay->set_image(l_selected_texture);

    //Prompt (For Blackouts / Look)
    m_prompt = new RPLabel(this, ao_app);
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


void DrPlayerListEntry::set_character(QString p_character, bool afkState)
{

  m_character = p_character;
  m_afk = afkState;
  QString characterIconPath = "";
  const QString afkBoarderImagePath = ao_app->find_theme_asset_path("char_border_afk.png");
  const QString nonAfkBoarderPath = ao_app->find_theme_asset_path("char_border.png");

  if(m_afk && FS::Checks::FileExists(afkBoarderImagePath))
  {
    pCharacterBorderDisplay->set_image(afkBoarderImagePath);
  }
  else
  {
    if (FS::Checks::FileExists(nonAfkBoarderPath))
      pCharacterBorderDisplay->set_image(nonAfkBoarderPath);
  }

  if(!m_CharacterOutfit.isEmpty())
  {
    characterIconPath = ao_app->get_character_path(m_character, "outfits/" + m_CharacterOutfit + "/char_icon.png");
    if(!FS::Checks::FileExists(characterIconPath))
    {
      characterIconPath = "";
    }
  }

  if(characterIconPath.isEmpty())
  {
    characterIconPath = ao_app->get_character_path(m_character, "char_icon.png");
  }

  if(FS::Checks::FileExists(characterIconPath))
  {
      ui_user_image->set_image(characterIconPath);
      if(m_afk && FS::Checks::FileExists(afkBoarderImagePath))
      {
        pCharacterBorderDisplay->set_image(afkBoarderImagePath);
      }
      else
      {
        const QString l_selected_texture = ao_app->get_character_path(p_character, "char_border.png");
        if (FS::Checks::FileExists(l_selected_texture)) pCharacterBorderDisplay->set_image(l_selected_texture);
      }

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

void DrPlayerListEntry::setOutfit(QString outfitName)
{
  m_CharacterOutfit = outfitName;
  set_character(m_character, m_afk);
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
  QUrl folderUrl = QUrl::fromLocalFile(FS::Paths::FindDirectory("characters/" + m_character));
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

void DrPlayerListEntry::sendLayerFront()
{
  ao_app->send_server_packet(DRPacket("PAIRL", {QString::number(1)}));
}

void DrPlayerListEntry::sendLayerBack()
{
  ao_app->send_server_packet(DRPacket("PAIRL", {QString::number(0)}));
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

  menu->addSeparator();

  if(user::partner::isUnpaired())
  {
    QAction *pairRequest = menu->addAction(localization::getText("PLAYER_LIST_PAIR"));
    connect(pairRequest, &QAction::triggered, this, &DrPlayerListEntry::sendPairRequest);
  }
  else
  {
    QMenu *pairMenu = menu->addMenu("Pair Options");

    QAction *frontAction = pairMenu->addAction("Move Front");
    connect(frontAction, &QAction::triggered, this, &DrPlayerListEntry::sendLayerFront);

    QAction *backAction = pairMenu->addAction("Move Back");
    connect(backAction, &QAction::triggered, this, &DrPlayerListEntry::sendLayerBack);

    QAction *unpairAction = pairMenu->addAction(localization::getText("PLAYER_LIST_UNPAIR"));
    connect(unpairAction, &QAction::triggered, this, &DrPlayerListEntry::sendUnpairRequest);
  }


  menu->addSeparator();

  QAction *openFolderAction = menu->addAction(localization::getText("OPEN_CHAR_FOLDER"));
  connect(openFolderAction, &QAction::triggered, this, &DrPlayerListEntry::openCharacterFolder);

  menu->addSeparator();

  if(!mURL.isEmpty())
  {
    QUrl url(mURL);
    QString label = "Open " + url.host() + " in Browser";
    QAction *browserAction = menu->addAction(label);
    connect(browserAction, &QAction::triggered, this, &DrPlayerListEntry::openBrowserURL);
  }

  if (!mHDID.isEmpty())
  {
    QString label = localization::getText("MOD_COPY_HDID") + " [" + mHDID + "]";
    QAction *copyHDID = menu->addAction(label);
    connect(copyHDID, &QAction::triggered, this, &DrPlayerListEntry::copyHDID);
  }

  if (!mIPID.isEmpty())
  {
    QString label = localization::getText("MOD_COPY_IPID") + " [" + mIPID + "]";
    QAction *copyIPID = menu->addAction(label);
    connect(copyIPID, &QAction::triggered, this, &DrPlayerListEntry::copyIPID);
  }

  QAction *copyIDAction = menu->addAction(localization::getText("PLAYER_LIST_ID"));
  connect(copyIDAction, &QAction::triggered, this, &DrPlayerListEntry::copyID);

  menu->popup(this->mapToGlobal(pos));
}
