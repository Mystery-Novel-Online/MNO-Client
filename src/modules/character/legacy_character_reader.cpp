#include "legacy_character_reader.h"
#include "aoapplication.h"
#include "commondefs.h"
#include "file_functions.h"
#include <QSettings>
#include <utils.h>

LegacyCharacterReader::LegacyCharacterReader()
{

}

QString LegacyCharacterReader::getShowname()
{
  return AOApplication::getInstance()->read_char_ini(mFolder, "options", "showname", mFolder).toString();
}

QString LegacyCharacterReader::getGender()
{
  return "male";
}

QString LegacyCharacterReader::getSide()
{
  return AOApplication::getInstance()->read_char_ini(mFolder, "options", "side", "wit").toString();
}

QString LegacyCharacterReader::drLookupKey(const QStringList &keyList, const QString &targetKey)
{
  const QString finalTargetKey = targetKey.toLower();
  for (const QString &i_key : qAsConst(keyList))
    if (i_key.toLower() == finalTargetKey)
      return i_key;
  return targetKey;
}

QVector<DREmote> LegacyCharacterReader::getEmotes()
{
  QVector<DREmote> r_emote_list;

  QStringList l_chr_list = AOApplication::getInstance()->get_char_include(mFolder);
  l_chr_list.append(mFolder);

#ifdef QT_DEBUG
  qDebug().noquote() << QString("Compiling char.ini for character <%1>").arg(mFolder);
#endif

  for (const QString &i_chr : l_chr_list)
  {
    if (!dir_exists(AOApplication::getInstance()->get_character_folder_path(i_chr)))
    {
      qWarning().noquote()
          << QString("Parent character <%1> not found, character <%2> cannot use it.").arg(i_chr, mFolder);
      continue;
    }
#ifdef QT_DEBUG
    qDebug().noquote() << QString("Adding <%1>").arg(i_chr);
#endif

    QSettings l_chrini(AOApplication::getInstance()->get_character_path(i_chr, CHARACTER_CHAR_INI), QSettings::IniFormat);
    l_chrini.setIniCodec("UTF-8");
    utils::QSettingsKeyFetcher l_fetcher(l_chrini);

    QStringList l_keys;
    { // recover all numbered keys, ignore words
      l_chrini.beginGroup(l_fetcher.lookup_group("emotions"));
      l_keys = l_chrini.childKeys();
      l_chrini.endGroup();

             // remove keywords
      l_keys.removeAll(drLookupKey(l_keys, "firstmode"));
      l_keys.removeAll(drLookupKey(l_keys, "number"));

             // remove all negative and non-numbers
      for (int i = 0; i < l_keys.length(); ++i)
      {
        const QString &i_key = l_keys.at(i);
        bool ok = false;
        const int l_num = i_key.toInt(&ok);
        if (ok && l_num >= 0)
          continue;
        l_keys.removeAt(i--);
      }

      std::stable_sort(l_keys.begin(), l_keys.end(), [](const QString &a, const QString &b) -> bool {
                         // if 0s are added at the beginning of the key, consider a whole number
                         if (a.length() < b.length())
                           return true;
                         return a.toInt() < b.toInt();
                       });
    }

    for (const QString &i_key : qAsConst(l_keys))
    {
      l_chrini.beginGroup(l_fetcher.lookup_group("emotions"));
      const QStringList l_emotions = l_chrini.value(i_key).toString().split("#", DR::KeepEmptyParts);
      l_chrini.endGroup();

      if (l_emotions.length() < 4)
      {
        qWarning().noquote() << QString("Emote <%2> of <%1>; emote is malformed.").arg(i_chr, i_key);
        continue;
      }
      enum EmoteField
      {
        Comment,
        Animation,
        Dialog,
        Modifier,
        DeskModifier,
      };

      DREmote l_emote;
      l_emote.key = i_key;
      l_emote.character = i_chr;
      l_emote.comment = l_emotions.at(Comment);
      l_emote.anim = l_emotions.at(Animation);
      l_emote.dialog = l_emotions.at(Dialog);
      l_emote.modifier = qMax(l_emotions.at(Modifier).toInt(), 0);
      if (DeskModifier < l_emotions.length())
        l_emote.desk_modifier = l_emotions.at(DeskModifier).toInt();

      l_chrini.beginGroup(l_fetcher.lookup_group("soundn"));
      l_emote.sound_file = l_chrini.value(i_key).toString();
      l_chrini.endGroup();

      l_chrini.beginGroup(l_fetcher.lookup_group("soundd"));
      if (l_chrini.contains(i_key))
      {
        l_emote.sound_delay = l_chrini.value(i_key).toInt();
      }
      else
      {
        l_chrini.endGroup();
        l_chrini.beginGroup(l_fetcher.lookup_group("soundt"));
        l_emote.sound_delay = l_chrini.value(i_key).toInt() * 60;
      }
      l_chrini.endGroup();
      l_emote.sound_delay = qMax(0, l_emote.sound_delay);

      l_chrini.beginGroup(l_fetcher.lookup_group("videos"));
      l_emote.video_file = l_chrini.value(i_key).toString();
      l_chrini.endGroup();

             // add the emote
      r_emote_list.append(l_emote);
    }
  }

  return r_emote_list;
}

QString LegacyCharacterReader::getEmoteButton(DREmote t_emote, bool t_enabled)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("emotions/button%1_off.png").arg(t_emote.key));

  if(t_enabled) l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("emotions/button%1_on.png").arg(t_emote.key));
  return l_texture;
}

QString LegacyCharacterReader::getSelectedImage(DREmote t_emote)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, "emotions/selected.png");
  return l_texture;
}

