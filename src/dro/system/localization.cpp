#include "localization.h"
#include <QDir>
#include <QFileInfo>
#include <QMap>
#include "dro/fs/fs_reading.h"
#include "dro/param/localization_reader.h"

QString s_currentLanguage = "English";
QStringList s_availableLanguages = { "English" };
QMap<QString, LocalizationReader*> s_languageData = {};

QMap<QString, QString> s_fallbackText =
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
  {"SFX_PREVIEW", "Preview"},
  {"SFX_FILENAME", "Insert filename"},
  {"SFX_CAPTION", "Insert caption"}
};

namespace dro::system::localization
{
  void initialize()
  {
    s_availableLanguages.clear();
    s_availableLanguages.append("English");
    QString l_localizationPath = FS::Paths::BasePath() + "localization/" ;
    QDir l_localizationDirectory(l_localizationPath);

    QStringList l_fileList = l_localizationDirectory.entryList(QStringList() << "*.json", QDir::Files);

    for(QString r_fileName : l_fileList)
    {
      QFileInfo l_fileInfo(l_localizationDirectory, r_fileName);

      LocalizationReader *l_newLocalization = new LocalizationReader(l_fileInfo.baseName());
      QString l_languageName = l_newLocalization->getLanguageName();
      if(!l_languageName.isEmpty())
      {
        s_languageData[l_languageName] = l_newLocalization;
        if(!s_availableLanguages.contains(l_languageName)) s_availableLanguages.append(l_languageName);
      };
    }

  }

  void switchLanguage(QString language)
  {
    s_currentLanguage = language;
  }

  QStringList getAvailableLanguages()
  {
    return s_availableLanguages;
  }

  QString getText(QString value)
  {
    if(s_languageData.contains(s_currentLanguage))
      if(s_languageData[s_currentLanguage]->containsLocalizationValue(value)) return s_languageData[s_currentLanguage]->getLocalizationValue(value);

    if(s_fallbackText.contains(value))
      return s_fallbackText[value];

    return "";
  }

  QString getCode()
  {
    if(s_languageData.contains(s_currentLanguage))
      return s_languageData[s_currentLanguage]->getCode();
    return "en";
  }

  QString getCredit()
  {
    if(s_languageData.contains(s_currentLanguage))
      return s_languageData[s_currentLanguage]->getCredit();
    return "";
  }

}

