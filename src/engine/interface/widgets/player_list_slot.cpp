#include "player_list_slot.h"

#include "theme.h"

#include "engine/fs/fs_reading.h"
#include "engine/system/localization.h"
#include "engine/network/metadata/user_metadata.h"
#include "modules/theme/thememanager.h"
#include "engine/fs/fs_characters.h"
#include "engine/interface/scenes/downloader_prompt.h"

#include "engine/discord/workshop_discord.h"

using namespace engine::network::metadata;
using namespace engine::system;

namespace {
constexpr int DEFAULT_HEIGHT = 50;
constexpr int STATUS_ICON_SIZE = 26;
constexpr int ICON_OFFSET = 5;
constexpr int TEXT_HEIGHT = 18;
constexpr int NAME_Y_OFFSET = 7;
constexpr int TYPING_Y_OFFSET = 27;
}

DrPlayerListEntry::DrPlayerListEntry(QWidget *parent, AOApplication *p_ao_app, int p_x, int p_y)
    : RPWidget("player_list_slot", parent)
{
  resetTransform();

  ao_app = p_ao_app;

  ui_showname = new RPLabel("player_list_showname", this);
  ui_typing = new RPLabel("player_list_typing", this);
  ui_user_image = new AOImageDisplay(this, ao_app);
  pCharacterBorderDisplay = new AOImageDisplay(this, ao_app);
  pStatusDisplay = new AOImageDisplay(this, ao_app);
  m_prompt = new RPLabel(this, ao_app);

  bool automaticScaling = height() == 0;

  this->move(p_x, p_y);
  if(automaticScaling)
  {
    refreshAutomatic(parent->size().width());
  }
  else
  {
    refreshManual(parent->size().width());
  }

  const QString lStatusImagePath = ao_app->find_theme_asset_path("player_list_status.png");
  if (FS::Checks::FileExists(lStatusImagePath)) pStatusDisplay->set_image(lStatusImagePath);


  const QString l_selected_texture = ao_app->find_theme_asset_path("char_border.png");
  if (FS::Checks::FileExists(l_selected_texture)) pCharacterBorderDisplay->set_image(l_selected_texture);

  set_stylesheet(m_prompt, "[PLAYER LIST PROMPT]", COURTROOM_STYLESHEETS_CSS, ao_app);

  ui_typing->setText("Typing...");

  ui_typing->hide();
  ui_showname->hide();
  ui_user_image->hide();
  pCharacterBorderDisplay->hide();
  pStatusDisplay->hide();
  m_prompt->hide();

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this ,&QWidget::customContextMenuRequested, this, &DrPlayerListEntry::showContextMenu);


  m_typingTimer = new QTimer(this);
  m_typingTimer->setSingleShot(true);
  m_typingTimer->setInterval(11000);

  connect(m_typingTimer, &QTimer::timeout, this, &DrPlayerListEntry::handleTypingTimeout);

}

void DrPlayerListEntry::refreshManual(int width)
{
  ui_showname->themeRefresh();
  set_stylesheet(ui_showname, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  ui_typing->themeRefresh();
  set_stylesheet(ui_typing, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  engine::system::theme::applyDimensions(ui_user_image, "player_list_icon", ThemeSceneType::SceneType_Courtroom);
  engine::system::theme::applyDimensions(pCharacterBorderDisplay, "player_list_border", ThemeSceneType::SceneType_Courtroom);
  engine::system::theme::applyDimensions(pStatusDisplay, "player_list_status", ThemeSceneType::SceneType_Courtroom);

  //Prompt (For Blackouts / Look)
  engine::system::theme::applyDimensions(m_prompt, "player_list_prompt", ThemeSceneType::SceneType_Courtroom);
  m_prompt->setWordWrap(true);
}


void DrPlayerListEntry::refreshAutomatic(int width)
{

  const double themeResize = ThemeManager::get().getResize();

  const int widgetHeight = static_cast<int>(DEFAULT_HEIGHT * themeResize);
  const int widgetWidth = width;
  const int statusIconSize = static_cast<int>(STATUS_ICON_SIZE * themeResize);

  this->resize(widgetWidth, widgetHeight);

  ui_showname->move(widgetHeight, 7);
  ui_showname->resize(widgetWidth-widgetHeight, 18);

  set_stylesheet(ui_showname, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  ui_typing->move(widgetHeight + (widgetWidth-widgetHeight) / 2, 27);
  ui_typing->resize((widgetWidth-widgetHeight) / 2, 18);

  set_stylesheet(ui_typing, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  int iconDimensionsoffset = (int)((float)5 * themeResize);
  int iconScale = (int)((float)40 * themeResize);
  ui_user_image->move(iconDimensionsoffset, iconDimensionsoffset);
  ui_user_image->resize(iconScale, iconScale);

  pCharacterBorderDisplay->move(0, 0);
  pCharacterBorderDisplay->resize(widgetHeight, widgetHeight);

  pStatusDisplay->move((int)((float)30 * themeResize),(int)((float)23 * themeResize));
  pStatusDisplay->resize(statusIconSize, statusIconSize);

  m_prompt->move(5, 5);
  m_prompt->resize(widgetWidth, widgetHeight);
  m_prompt->setWordWrap(true);

}

void DrPlayerListEntry::toggleTyping(bool status)
{
  if (status)
  {
    ui_typing->show();
    m_typingTimer->start();
  }
  else
  {
    ui_typing->hide();
  }
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
    characterIconPath = fs::characters::getFilePath(m_character, "outfits/" + m_CharacterOutfit + "/char_icon.png");
    if(!FS::Checks::FileExists(characterIconPath))
    {
      characterIconPath = "";
    }
  }

  if(characterIconPath.isEmpty())
  {
    characterIconPath = fs::characters::getFilePath(m_character, "char_icon.png");
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
        const QString l_selected_texture = fs::characters::getFilePath(p_character, "char_border.png");
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
  ui_typing->hide();
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
  m_clientId = id;
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

int DrPlayerListEntry::clientId()
{
  return m_clientId;
}

void DrPlayerListEntry::addDiscordFriend()
{
  WorkshopDiscord::getInstance().sendFriendRequest(m_discord);
}

void DrPlayerListEntry::messageDiscordFriend()
{
  bool ok;
  QString text = QInputDialog::getText(nullptr,
                                       "Message User (TEST)",
                                       "Insert test message here:",
                                       QLineEdit::Normal,
                                       "",
                                       &ok);

  if (ok && !text.isEmpty()) {
    WorkshopDiscord::getInstance().sendPrivateMessage(m_discord, text);
  }
}

void DrPlayerListEntry::openCharacterFolder()
{
  QUrl folderUrl = QUrl::fromLocalFile(FS::Paths::FindDirectory("characters/" + m_character));
  QDesktopServices::openUrl(folderUrl);
}

void DrPlayerListEntry::openBrowserURL()
{
  DownloaderPrompt::StartDownload(mURL, "packages/Workshop Downloads", m_character);
}

void DrPlayerListEntry::sendPairRequest()
{
  ao_app->send_server_packet(DRPacket("PR", {QString::number(m_clientId)}));
}

void DrPlayerListEntry::sendUnpairRequest()
{
  ao_app->send_server_packet(DRPacket("UPR", {QString::number(m_clientId)}));
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

  clipboard->setText(QString::number(m_clientId));
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

void DrPlayerListEntry::followPlayer()
{
  AOApplication::getInstance()->get_courtroom()->send_ooc_packet("/follow " + QString::number(m_clientId));
}

void DrPlayerListEntry::handleTypingTimeout()
{
  toggleTyping(false);
}

void DrPlayerListEntry::showContextMenu(QPoint pos)
{
  QMenu *menu = new QMenu(this);
  menu->addAction("[" + QString::number(m_clientId) + "] " + m_showname);

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

  QAction *followUserAction = menu->addAction("Follow Player");
  connect(followUserAction, &QAction::triggered, this, &DrPlayerListEntry::followPlayer);

  menu->addSeparator();

  QAction *openFolderAction = menu->addAction(localization::getText("OPEN_CHAR_FOLDER"));
  connect(openFolderAction, &QAction::triggered, this, &DrPlayerListEntry::openCharacterFolder);

  menu->addSeparator();

  if(!mURL.isEmpty())
  {
    QUrl url(mURL);
    QString label = mURL.endsWith("/repo") || mURL.endsWith("/collection") ? "Download Workshop Character" : "Open " + url.host() + " in Browser";
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

    if(!m_discord.isEmpty())
    {
      QMenu *pairMenu = menu->addMenu("Discord (Debug)");

      QAction *discFriendAction = pairMenu->addAction("Add Friend");
      connect(discFriendAction, &QAction::triggered, this, &DrPlayerListEntry::addDiscordFriend);

      QAction *discMessageAction = pairMenu->addAction("Send Message");
      connect(discMessageAction, &QAction::triggered, this, &DrPlayerListEntry::messageDiscordFriend);

      QAction *discFriendWhitelist = pairMenu->addAction("Add to Current Whitelist");
    }
  }

  QAction *copyIDAction = menu->addAction(localization::getText("PLAYER_LIST_ID"));
  connect(copyIDAction, &QAction::triggered, this, &DrPlayerListEntry::copyID);

  menu->popup(this->mapToGlobal(pos));
}
