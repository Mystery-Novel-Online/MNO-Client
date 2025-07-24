#include "fs_characters.h"
#include "fs_reading.h"

namespace{
const QStringList SPRITE_PATH_BLACKLIST = { "char_icon.png", "showname.png", "emotions" };
}

QString fs::characters::getSpritePath(QString p_character, QString p_emote, QString p_prefix, bool p_use_placeholder)
{
  bool l_valid = true;

  for (const QString &i_black : SPRITE_PATH_BLACKLIST)
  {
    if (p_emote.startsWith(i_black, Qt::CaseInsensitive))
    {
      l_valid = false;
      break;
    }
  }

  QStringList l_file_name_list;
  for (const QString &i_extension : FS::Formats::SupportedImages())
  {
    if (!p_prefix.isEmpty())
    {
      if (p_emote.contains("outfits/"))
      {
        int lastSlashIndex = p_emote.lastIndexOf('/');
        l_file_name_list.append(p_emote.left(lastSlashIndex + 1) + p_prefix + p_emote.mid(lastSlashIndex + 1) + i_extension);
      }
      else
      {
        l_file_name_list.append(p_prefix + p_emote + i_extension);
      }

    }
    l_file_name_list.append(p_emote + i_extension);
  }

  QString l_file_path;
  if (l_valid)
  {
    QStringList l_file_path_list;
    for (const QString &i_chr_name : AOApplication::getInstance()->get_char_include_tree(p_character))
    {
      for (const QString &i_file_name : qAsConst(l_file_name_list))
      {
        l_file_path_list.append(getFilePath(i_chr_name, i_file_name));
      }
    }

    for (const QString &i_file_path : qAsConst(l_file_path_list))
    {
      const QString l_resolved_file_path = AOApplication::getInstance()->find_asset_path(i_file_path);
      if (!l_resolved_file_path.isEmpty())
      {
        l_file_path = l_resolved_file_path;
        break;
      }
    }
  }

  if (l_file_path.isEmpty() && p_use_placeholder)
  {
    l_file_path = AOApplication::getInstance()->find_theme_asset_path("placeholder", FS::Formats::AnimatedImages());
  }

  if (l_file_path.isEmpty())
  {
    qWarning() << "error: character animation not found"
               << "character:" << p_character << "emote:" << p_emote << "prefix:" << p_prefix;
  }

  return l_file_path;
}

QString fs::characters::getSpritePathPre(QString character, QString emote)
{
  return getSpritePath(character, emote, QString{}, false);
}

QString fs::characters::getSpritePathTalk(QString character, QString emote)
{
  return getSpritePath(character, emote, "(b)", true);
}

QString fs::characters::getSpritePathIdle(QString character, QString emote)
{
  return getSpritePath(character, emote, "(a)", true);
}

QString fs::characters::getDirectoryPath(const QString &character)
{
  return FS::Paths::FindDirectory("characters/" + character);
}

QString fs::characters::getFilePath(const QString &character, const QString &file)
{
  return getDirectoryPath(character) + "/" + file;
}

QString fs::characters::getSpritePath(const std::string &p_character, const std::string &p_emote, const std::string &p_prefix, bool p_use_placeholder)
{
  return getSpritePath(QString::fromStdString(p_character), QString::fromStdString(p_emote), QString::fromStdString(p_prefix), p_use_placeholder);
}
