#include "actor_loader.h"
#include <utils.h>
#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_characters.h"
#include <rolechat/actor/ActorOutfit.h>
#include <rolechat/actor/JsonActorData.h>

void LegacyActorReader::load(const std::string& folder, const std::string& path)
{
  QString qFolder = QString::fromStdString(folder);
  setFolder(folder);
  auto* app = AOApplication::getInstance();
  setShowname(app->read_char_ini(qFolder, "options", "showname", qFolder).toString().toStdString());
  setSide(app->read_char_ini(qFolder, "options", "side", "wit").toString().toStdString());
  setGender("male");
}

QString LegacyActorReader::DRLookupKey(const QStringList &keys, const QString &target)
{
  const QString targetLower = target.toLower();
  for (const QString& key : keys)
    if (key.toLower() == targetLower)
      return key;
  return target;
}

std::vector<ActorEmote> LegacyActorReader::emotes()
{
  std::vector<ActorEmote> r_emote_list;

  QStringList l_chr_list = AOApplication::getInstance()->get_char_include(QString::fromStdString(folder()));
  l_chr_list.append(QString::fromStdString(folder()));

#ifdef QT_DEBUG
  qDebug().noquote() << QString("Compiling char.ini for character <%1>").arg(QString::fromStdString(folder()));
#endif

  for (const QString &i_chr : l_chr_list)
  {
    if (!FS::Checks::DirectoryExists(fs::characters::getDirectoryPath(i_chr)))
    {
      qWarning().noquote()
      << QString("Parent character <%1> not found, character <%2> cannot use it.").arg(i_chr, QString::fromStdString(folder()));
      continue;
    }
#ifdef QT_DEBUG
    qDebug().noquote() << QString("Adding <%1>").arg(i_chr);
#endif

    QSettings l_chrini(fs::characters::getFilePath(i_chr, CHARACTER_CHAR_INI), QSettings::IniFormat);
    l_chrini.setIniCodec("UTF-8");
    utils::QSettingsKeyFetcher l_fetcher(l_chrini);

    QStringList l_keys;
    { // recover all numbered keys, ignore words
      l_chrini.beginGroup(l_fetcher.lookup_group("emotions"));
      l_keys = l_chrini.childKeys();
      l_chrini.endGroup();

             // remove keywords
      l_keys.removeAll(DRLookupKey(l_keys, "firstmode"));
      l_keys.removeAll(DRLookupKey(l_keys, "number"));

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
      const QStringList l_emotions = l_chrini.value(i_key).toString().split("#", DR::SplitBehavior::KeepEmptyParts);
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

      ActorEmote l_emote;
      l_emote.key = i_key.toStdString();
      l_emote.character = i_chr.toStdString();
      l_emote.comment = l_emotions.at(Comment).toStdString();
      l_emote.anim = l_emotions.at(Animation).toStdString();
      l_emote.dialog = l_emotions.at(Dialog).toStdString();
      l_emote.emoteName = l_emotions.at(Dialog).toStdString();
      l_emote.modifier = qMax(l_emotions.at(Modifier).toInt(), 0);
      if (DeskModifier < l_emotions.length())
        l_emote.desk_modifier = l_emotions.at(DeskModifier).toInt();

      l_chrini.beginGroup(l_fetcher.lookup_group("soundn"));
      l_emote.sound_file = l_chrini.value(i_key).toString().toStdString();
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
      l_emote.video_file = l_chrini.value(i_key).toString().toStdString();
      l_chrini.endGroup();

             // add the emote
      r_emote_list.push_back(l_emote);
    }
  }

  return r_emote_list;
}

std::string LegacyActorReader::buttonImage(const ActorEmote& t_emote, bool t_enabled) const
{
  return t_enabled ? QString("emotions/button%1_on.png").arg(QString::fromStdString(t_emote.key)).toStdString() : QString("emotions/button%1_off.png").arg(QString::fromStdString(t_emote.key)).toStdString();
}

std::string LegacyActorReader::selectedImage(const ActorEmote& t_emote) const
{
  return "emotions/selected.png";
}

