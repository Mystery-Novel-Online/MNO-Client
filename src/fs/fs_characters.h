#ifndef FS_CHARACTERS_H
#define FS_CHARACTERS_H

namespace engine::fs::characters
{
  QString getSpritePath(const QString& p_character, const QString& p_emote, const QString& p_prefix, bool p_use_placeholder);
  QString getSpritePath(const std::string& p_character, const std::string& p_emote, const std::string& p_prefix, bool p_use_placeholder);
  QString getSpritePathPre(const QString& character, const QString& emote);
  QString getSpritePathTalk(const QString& character, const QString& emote);
  QString getSpritePathIdle(const QString& character, const QString& emote);

  QString getDirectoryPath(const QString& character);
  QString getFilePath(const QString& character, const QString& file);
}


#endif // FS_CHARACTERS_H
