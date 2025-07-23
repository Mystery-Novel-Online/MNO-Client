#ifndef FS_CHARACTERS_H
#define FS_CHARACTERS_H

namespace dro::fs::characters
{
  const QString getSpritePath(QString p_character, QString p_emote, QString p_prefix, bool p_use_placeholder);
  const QString getSpritePathPre(QString character, QString emote);
  const QString getSpritePathTalk(QString character, QString emote);
  const QString getSpritePathIdle(QString character, QString emote);

  const QString getDirectoryPath(const QString& character);
  const QString getFilePath(const QString& character, const QString& file);
}


#endif // FS_CHARACTERS_H
