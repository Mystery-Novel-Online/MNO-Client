#ifndef FS_CHARACTERS_H
#define FS_CHARACTERS_H

namespace engine::fs::characters
{
  QString getSpritePath(QString p_character, QString p_emote, QString p_prefix, bool p_use_placeholder);
  QString getSpritePath(const std::string& p_character, const std::string& p_emote, const std::string& p_prefix, bool p_use_placeholder);
  QString getSpritePathPre(QString character, QString emote);
  QString getSpritePathTalk(QString character, QString emote);
  QString getSpritePathIdle(QString character, QString emote);

  QString getDirectoryPath(const QString& character);
  QString getFilePath(const QString& character, const QString& file);
}


#endif // FS_CHARACTERS_H
