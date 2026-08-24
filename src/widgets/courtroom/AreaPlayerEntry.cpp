#include "AreaPlayerEntry.h"

#include "engine/system/user_database.h"
#include "theme.h"

#include "engine/fs/fs_reading.h"
#include "engine/system/localization.h"
#include "engine/network/metadata/user_metadata.h"
#include "modules/theme/legacythememanager.h"
#include "engine/fs/fs_characters.h"
#include "engine/interface/scenes/downloader_prompt.h"

#include "engine/discord/workshop_discord.h"

#include <rolechat/filesystem/RCDir.h>

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

AreaPlayerEntry::AreaPlayerEntry(QWidget *parent, AOApplication *p_ao_app, int p_x, int p_y, const DrPlayer& player)
  : RPWidget("player_list_slot", parent)
  , m_playerData(player) {
  resetTransform();

  ao_app = p_ao_app;

  u_shownameDisplay = new RPLabel("player_list_showname", this);
  u_typingIndicator = new RPLabel("player_list_typing", this);
  u_playerIcon = new AOImageDisplay(this, ao_app);
  u_playerIconBorder = new AOImageDisplay(this, ao_app);
  u_statusDisplay = new AOImageDisplay(this, ao_app);
  u_updateDisplay = new AOImageDisplay(this, ao_app);
  u_areaPrompt = new RPLabel(this, ao_app);

  bool automaticScaling = height() == 0;

  this->move(p_x, p_y);
  if(automaticScaling) {
    refreshAutomatic(parent->size().width());
  }
  else {
    refreshManual(parent->size().width());
  }

  themeReload();

  u_typingIndicator->setText("Typing...");

  u_typingIndicator->hide();
  u_updateDisplay->hide();
  u_shownameDisplay->hide();
  u_playerIcon->hide();
  u_playerIconBorder->hide();
  u_statusDisplay->hide();
  u_areaPrompt->hide();

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this ,&QWidget::customContextMenuRequested, this, &AreaPlayerEntry::showContextMenu);


  m_typingTimer = new QTimer(this);
  m_typingTimer->setSingleShot(true);
  m_typingTimer->setInterval(6000);

  connect(m_typingTimer, &QTimer::timeout, this, &AreaPlayerEntry::handleTypingTimeout);
  updateData(player, p_y);

}

void AreaPlayerEntry::themeReload()
{
  const QString imagePathStatus = ao_app->find_theme_asset_path("player_list_status.png");
  const QString imagePathUpdate = ao_app->find_theme_asset_path("player_content_update.png");
  const QString imagePathBorder = ao_app->find_theme_asset_path("char_border.png");

  if (FS::Checks::FileExists(imagePathStatus)) {
    u_statusDisplay->set_image(imagePathStatus);
  }

  if (FS::Checks::FileExists(imagePathUpdate)) {
    u_updateDisplay->set_image(imagePathUpdate);
  }

  if (FS::Checks::FileExists(imagePathBorder)) {
    u_playerIconBorder->set_image(imagePathBorder);
  }

  set_stylesheet(u_areaPrompt, "[PLAYER LIST PROMPT]", COURTROOM_STYLESHEETS_CSS, ao_app);
}

void AreaPlayerEntry::updateData(const DrPlayer &player, int y)
{
  this->move(1, y);
  m_playerData = player;

  if(player.data.showname != m_showname) {
    setShowname(player.data.showname);
  }

  if(player.data.character != m_playerCharacter || player.data.outfit != m_playerOutfit || player.data.afk != m_afk || m_missingPlayerIcon) {
    m_playerCharacter = player.data.character;
    setOutfit(player.data.outfit);
    setContentVersion(player.data.contentVersion);
  }
  setStatus(player.data.status);
}

void AreaPlayerEntry::refreshManual(int width)
{
  u_shownameDisplay->themeRefresh();
  set_stylesheet(u_shownameDisplay, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  u_typingIndicator->themeRefresh();
  set_stylesheet(u_typingIndicator, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  engine::system::theme::applyDimensions(u_playerIcon, "player_list_icon", ThemeSceneType::SceneType_Courtroom);
  engine::system::theme::applyDimensions(u_playerIconBorder, "player_list_border", ThemeSceneType::SceneType_Courtroom);
  engine::system::theme::applyDimensions(u_statusDisplay, "player_list_status", ThemeSceneType::SceneType_Courtroom);
  engine::system::theme::applyDimensions(u_updateDisplay, "player_list_update", ThemeSceneType::SceneType_Courtroom);


  //Prompt (For Blackouts / Look)
  engine::system::theme::applyDimensions(u_areaPrompt, "player_list_prompt", ThemeSceneType::SceneType_Courtroom);
  u_areaPrompt->setWordWrap(true);
}


void AreaPlayerEntry::refreshAutomatic(int width) {
  const double themeResize = LegacyThemeManager::get().getResize();

  const int widgetHeight = static_cast<int>(DEFAULT_HEIGHT * themeResize);
  const int widgetWidth = width;
  const int statusIconSize = static_cast<int>(STATUS_ICON_SIZE * themeResize);

  this->resize(widgetWidth, widgetHeight);

  u_shownameDisplay->move(widgetHeight, 7);
  u_shownameDisplay->resize(widgetWidth-widgetHeight, 18);

  set_stylesheet(u_shownameDisplay, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  u_typingIndicator->move(widgetHeight + (widgetWidth-widgetHeight) / 2, 27);
  u_typingIndicator->resize((widgetWidth-widgetHeight) / 2, 18);

  set_stylesheet(u_typingIndicator, "[PLAYER NAME]", COURTROOM_STYLESHEETS_CSS, ao_app);

  int iconDimensionsoffset = (int)((float)5 * themeResize);
  int iconScale = (int)((float)40 * themeResize);
  u_playerIcon->move(iconDimensionsoffset, iconDimensionsoffset);
  u_playerIcon->resize(iconScale, iconScale);

  u_playerIconBorder->move(0, 0);
  u_playerIconBorder->resize(widgetHeight, widgetHeight);


  u_updateDisplay->move((int)((float)30 * themeResize),(int)((float)23 * themeResize));
  u_updateDisplay->resize(statusIconSize, statusIconSize);

  u_statusDisplay->move((int)((float)30 * themeResize),(int)((float)23 * themeResize));
  u_statusDisplay->resize(statusIconSize, statusIconSize);

  u_areaPrompt->move(5, 5);
  u_areaPrompt->resize(widgetWidth, widgetHeight);
  u_areaPrompt->setWordWrap(true);

}

void AreaPlayerEntry::toggleTyping(bool status)
{
  if (status) {
    u_typingIndicator->show();
    m_typingTimer->start();
  }
  else {
    u_typingIndicator->hide();
  }
}


void AreaPlayerEntry::setCharacter(const QString& a_character, bool afkState) {
  m_playerCharacter = a_character;
  m_afk = afkState;
  QString characterIconPath = "";
  const QString afkBoarderImagePath = ao_app->find_theme_asset_path("char_border_afk.png");
  const QString nonAfkBoarderPath = ao_app->find_theme_asset_path("char_border.png");

  u_playerIcon->setAlpha("playerlist_alpha");

  if(m_afk && FS::Checks::FileExists(afkBoarderImagePath)) {
    u_playerIconBorder->set_image(afkBoarderImagePath);
  }
  else {
    if (FS::Checks::FileExists(nonAfkBoarderPath)) {
      u_playerIconBorder->set_image(nonAfkBoarderPath);
    }
  }

  if(!m_playerOutfit.isEmpty()) {
    characterIconPath = engine::fs::characters::getFilePath(m_playerCharacter, "outfits/" + m_playerOutfit + "/char_icon.png");
    if(!FS::Checks::FileExists(characterIconPath)) {
      characterIconPath = "";
    }
  }

  if(characterIconPath.isEmpty()) {
    characterIconPath = engine::fs::characters::getFilePath(m_playerCharacter, "char_icon.png");
  }

  if(FS::Checks::FileExists(characterIconPath)) {
    u_playerIcon->set_image(characterIconPath);
    if(m_afk && FS::Checks::FileExists(afkBoarderImagePath)) {
      u_playerIconBorder->set_image(afkBoarderImagePath);
    }
    else {
      const QString l_selected_texture = engine::fs::characters::getFilePath(a_character, "char_border.png");
      if (FS::Checks::FileExists(l_selected_texture)) {
        u_playerIconBorder->set_image(l_selected_texture);
      }
    }
  }
  else {
    QString l_missing_char_image = ao_app->find_theme_asset_path("missing_char.png");
    if (!l_missing_char_image.isEmpty()) {
      u_playerIcon->set_theme_image("missing_char.png");
      m_missingPlayerIcon = true;
    }
  }

  u_playerIcon->show();
  u_playerIconBorder->show();
  u_areaPrompt->hide();
}

void AreaPlayerEntry::setOutfit(const QString& outfitName)
{
  m_playerOutfit = outfitName;
  setCharacter(m_playerData.data.character, m_playerData.data.afk);
}

void AreaPlayerEntry::setShowname(const QString& showname)
{
  u_shownameDisplay->show();
  u_areaPrompt->hide();
  m_showname = showname;
  u_shownameDisplay->setText(showname);

}

void AreaPlayerEntry::setReason(const QString& a_reasonText)
{
  u_shownameDisplay->hide();
  u_typingIndicator->hide();
  u_playerIcon->hide();
  u_playerIconBorder->hide();
  u_areaPrompt->show();

  u_areaPrompt->setText(a_reasonText);
}

void AreaPlayerEntry::setStatus(const QString& a_statusText)
{
  if(!a_statusText.isEmpty()) {
    setToolTip(a_statusText);
    u_statusDisplay->show();
  }
}

void AreaPlayerEntry::setContentVersion(int versionNumber)
{
  if(versionNumber > GetDB().workshopUpdateTime(m_playerCharacter.toStdString())) {
    u_updateDisplay->show();
    u_statusDisplay->hide();
  }
  else {
    u_updateDisplay->hide();
  }
}

void AreaPlayerEntry::addDiscordFriend()
{
  WorkshopDiscord::getInstance().sendFriendRequest(m_playerData.data.discordSnowflake);
}

void AreaPlayerEntry::messageDiscordFriend()
{
  bool ok;
  QString text = QInputDialog::getText(nullptr,
                                       "Message User (TEST)",
                                       "Insert test message here:",
                                       QLineEdit::Normal,
                                       "",
                                       &ok);

  if (ok && !text.isEmpty()) {
    WorkshopDiscord::getInstance().sendPrivateMessage(m_playerData.data.discordSnowflake, text);
  }
}

void AreaPlayerEntry::openCharacterFolder() {
  QUrl folderUrl = QUrl::fromLocalFile(engine::fs::characters::getDirectoryPath(m_playerCharacter));
  QDesktopServices::openUrl(folderUrl);
}

void AreaPlayerEntry::openBrowserURL() {
  DownloaderPrompt::StartDownload(m_playerData.data.contentUrl, "packages/Workshop Downloads", m_playerCharacter, DOWNLOAD_PlayerList);
}

void AreaPlayerEntry::sendPairRequest() {
  ao_app->send_server_packet(DRPacket("PR", {QString::number(m_playerData.data.id)}));
}

void AreaPlayerEntry::sendUnpairRequest() {
  ao_app->send_server_packet(DRPacket("UPR", {QString::number(m_playerData.data.id)}));
}

void AreaPlayerEntry::sendLayerFront() {
  ao_app->send_server_packet(DRPacket("PAIRL", {QString::number(1)}));
}

void AreaPlayerEntry::sendLayerBack() {
  ao_app->send_server_packet(DRPacket("PAIRL", {QString::number(0)}));
}

void AreaPlayerEntry::copyID() {
  QGuiApplication::clipboard()->setText(QString::number(m_playerData.data.id));
}

void AreaPlayerEntry::copyHDID() {
  QGuiApplication::clipboard()->setText(m_playerData.data.modHDID);
}

void AreaPlayerEntry::copyIPID() {
  QGuiApplication::clipboard()->setText(m_playerData.data.modIPID);
}

void AreaPlayerEntry::followPlayer() {
  AOApplication::getInstance()->get_courtroom()->send_ooc_packet("/follow " + QString::number(m_playerData.data.id));
}

void AreaPlayerEntry::handleTypingTimeout() {
  toggleTyping(false);
}

void AreaPlayerEntry::showContextMenu(QPoint pos) {
  QMenu *menu = new QMenu(this);
  QMenu *playerMenu = menu->addMenu("[" + QString::number(m_playerData.data.id) + "] " + m_showname);
  menu->addSeparator();

  QMenu *pairMenu = menu->addMenu("Pair Options");

  if(user::partner::isUnpaired()) {
    QAction *pairRequest = pairMenu->addAction(localization::getText("PLAYER_LIST_PAIR"));
    connect(pairRequest, &QAction::triggered, this, &AreaPlayerEntry::sendPairRequest);
  }
  else {
    QAction *frontAction = pairMenu->addAction("Move Front");
    QAction *backAction = pairMenu->addAction("Move Back");
    QAction *unpairAction = pairMenu->addAction(localization::getText("PLAYER_LIST_UNPAIR"));

    connect(frontAction, &QAction::triggered, this, &AreaPlayerEntry::sendLayerFront);
    connect(backAction, &QAction::triggered, this, &AreaPlayerEntry::sendLayerBack);
    connect(unpairAction, &QAction::triggered, this, &AreaPlayerEntry::sendUnpairRequest);
  }

  QAction *followUserAction = menu->addAction("Follow Player");
  connect(followUserAction, &QAction::triggered, this, &AreaPlayerEntry::followPlayer);

  menu->addSeparator();

  if(!m_playerData.data.contentUrl.isEmpty()) {
    QUrl url(m_playerData.data.contentUrl);
    QString label = m_playerData.data.contentUrl.endsWith("/repo") || m_playerData.data.contentUrl.endsWith("/collection") || m_playerData.data.contentUrl.endsWith("/content")? "Download Character" : "Open " + url.host() + " in Browser";
    QAction *browserAction = menu->addAction(label);
    connect(browserAction, &QAction::triggered, this, &AreaPlayerEntry::openBrowserURL);
  }

  if(rolechat::fs::RCDir("characters/" + m_playerCharacter.toStdString()).exists()) {
    QAction *openFolderAction = menu->addAction(localization::getText("OPEN_CHAR_FOLDER"));
    connect(openFolderAction, &QAction::triggered, this, &AreaPlayerEntry::openCharacterFolder);
  }

  menu->addSeparator();

  QAction *copyIDAction = playerMenu->addAction(localization::getText("PLAYER_LIST_ID"));
  connect(copyIDAction, &QAction::triggered, this, &AreaPlayerEntry::copyID);

  if (!m_playerData.data.modHDID.isEmpty()) {
    QString label = localization::getText("MOD_COPY_HDID") + " [" + m_playerData.data.modHDID + "]";
    QAction *copyHDID = playerMenu->addAction(label);
    connect(copyHDID, &QAction::triggered, this, &AreaPlayerEntry::copyHDID);
  }

  if (!m_playerData.data.modIPID.isEmpty()) {
    QString label = localization::getText("MOD_COPY_IPID") + " [" + m_playerData.data.modIPID + "]";
    QAction *copyIPID = playerMenu->addAction(label);
    connect(copyIPID, &QAction::triggered, this, &AreaPlayerEntry::copyIPID);

    if(!m_playerData.data.discordSnowflake.isEmpty()) {
      QMenu *pairMenu = menu->addMenu("Discord (Debug)");

      QAction *discFriendAction = pairMenu->addAction("Add Friend");
      QAction *discMessageAction = pairMenu->addAction("Send Message");
      QAction *discFriendAllowList = pairMenu->addAction("Add to Current Allow List");

      connect(discFriendAction, &QAction::triggered, this, &AreaPlayerEntry::addDiscordFriend);
      connect(discMessageAction, &QAction::triggered, this, &AreaPlayerEntry::messageDiscordFriend);
    }
  }


  menu->popup(this->mapToGlobal(pos));
}
