#ifndef LOCALIZATIONMANAGER_H
#define LOCALIZATIONMANAGER_H

#include <QString>
#include <QMap>

#include <modules/json/localization_reader.h>

class LocalizationManager
{
public:
  LocalizationManager(const LocalizationManager&) = delete;

  static LocalizationManager& get()
  {
    return s_Instance;
  }

  void execLoadLanguages();
  void setLanguage(QString t_languageName);
  QStringList getLanguageNames();

  QString getLocalizationText(QString t_value);
  QString getLocalizationCode();
  QString getLocalizationCredit();


private:
  LocalizationManager() {}
  static LocalizationManager s_Instance;

  QString mSelectedLanguageName = "English";

  QMap<QString, LocalizationReader*> mLanguages = {};
  QStringList mLanguageNames = { "English" };

  QMap<QString, QString> mDefaultText =
      {
          {"TITLE_PRE", "Pre"},
          {"TITLE_FLIP", "Flip"},
          {"TITLE_HIDE", "Hidden"},
          {"TITLE_LOOK", "Look"},
          {"TOOLTIP_PRE", "Enables pre-animations and sound effects."},
          {"TOOLTIP_FLIP", "Flips the character sprite when you send a message."},
          {"TOOLTIP_HIDE", "Hides the character sprite when you send a message."},
          {"CHATBOX_IC", "Say something in-character."},
          {"CHATBOX_OOC", "Say something out-of-character."},
          {"TEXTBOX_OOC_NAME", "Name"},
          {"TEXTBOX_MUSIC", "Music Filter"},
          {"TEXTBOX_AREA", "Area Filter"},
          {"TEXTBOX_SHOWNAME", "Showname"},
          {"DEFAULT", "Default"},
          {"POS_WIT", "Witness"},
          {"POS_DEF", "Defense"},
          {"POS_PRO", "Prosecutor"},
          {"POS_JUD", "Judge"},
          {"POS_DEF_ASS", "Defense Assistant"},
          {"POS_PRO_ASS", "Prosecutor Assistant"},
          {"SFX_SILENCE", "Silence"},
          {"CHAT_TALK", "Talk"},
          {"CHAT_SHOUT", "Shout"},
          {"CHAT_THINK", "Think"},
          {"COLOR_WHITE", "White"},
          {"COLOR_GREEN", "Green"},
          {"COLOR_RED", "Red"},
          {"COLOR_ORANGE", "Orange"},
          {"COLOR_BLUE", "Blue"},
          {"COLOR_YELLOW", "Yellow"},
          {"COLOR_PURPLE", "Purple"},
          {"COLOR_PINK", "Pink"},
          {"COLOR_RAINBOW", "Rainbow"},
          {"CSS_DISCONNECT", "Disconnect"},
          {"CSS_SPECTATE", "Spectate"},
          {"REFRESH", "Refresh"},
          {"CSS_FAVORITES", "Favorites"},
          {"CSS_RANDOM", "Random"},
          {"CSS_SEARCH", "Search for a Character"},
          {"OOC_EMPTY", "You cannot send empty messages."},
          {"PING_MODS", "Ping Mods"},
          {"TAB_CHAT", "Chat"},
          {"TAB_GM", "GM"},
          {"TAB_AREA", "Area"},
          {"LOBBY_TITLE", "Server Select"},
          {"SERVER_FAVORITES_ADD", "Add"},
          {"SERVER_FAVORITES_EDIT", "Edit"},
          {"SERVER_FAVORITES_UP", "Move Up"},
          {"SERVER_FAVORITES_DOWN", "Move Down"},
          {"SERVER_FAVORITES_REMOVE", "Remove"},
          {"CONNECTION_NOT", "Choose a server."},
          {"CONNECTION_CONNECTING", "Connecting to server."},
          {"CONNECTION_CONNECTED", "Connected to server."},
          {"CONNECTION_JOINED", "Joined server."},
          {"CONNECTION_TIMEDOUT", "Failed to connect to server."},
          {"OPEN_CHAR_FOLDER", "Open Character Folder"},
          {"PLAYER_LIST_PAIR", "Send Pair Request"},
          {"PLAYER_LIST_UNPAIR", "Unpair from Partner"},
          {"PLAYER_LIST_URL", "Open %1 in Browser"},
          {"PLAYER_LIST_ID", "Copy Player ID"},
          {"MOD_COPY_HDID", "Copy HDID"},
          {"MOD_COPY_IPID", "Copy IPID"},
          {"REASON_BLACKOUT", "You can't see anyone as the lights are currently off."},
          {"REASON_PENDING", "There appears to be people in the area."},
          {"REASON_DISABLED", "The Player List has been disabled by the GM."},
          {"CSS_FAVORITES_ADD", "Add to Favorites"},
          {"CSS_FAVORITES_REMOVE", "Remove from Favorites"},
          {"NOTICE_DISCONNECT", "Disconnected from server."},
          {"NOTICE_BANNED", "You have been banned."},
          {"NOTICE_BANNED_2", "You are banned on this server."},
          {"NOTICE_KICKED", "You have been kicked."},
          {"NOTICE_MISSING", "Could not find %1"},
          {"", ""},
          {"", ""}
  };

};

#endif // LOCALIZATIONMANAGER_H
