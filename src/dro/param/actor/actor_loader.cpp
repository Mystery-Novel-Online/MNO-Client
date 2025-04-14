#include "actor_loader.h"
#include "file_functions.h"
#include "aoapplication.h"

#include "commondefs.h"
#include "file_functions.h"
#include <QSettings>
#include <utils.h>

#include <QDir>

static QMap<QString, ActorData*> s_CachedCharacters;

ActorData *ActorLoader::GetCharacter(QString folder)
{
  if(!s_CachedCharacters.contains(folder))
  {
    QString charaConfigPath = AOApplication::getInstance()->get_character_path(folder, "char.json");
    ActorData *characterData;
    if(file_exists(charaConfigPath))
    {
      characterData = new ActorDataReader();
    }
    else
    {
      characterData = new LegacyActorReader();
    }

    characterData->loadActor(folder);
    s_CachedCharacters[folder] = characterData;
  }

  return s_CachedCharacters[folder];
}

QString ActorData::getEmoteButton(DREmote t_emote, bool t_enabled)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("emotions/button%1_off.png").arg(t_emote.key));

  if(t_enabled) l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("emotions/button%1_on.png").arg(t_emote.key));
  return l_texture;
}

QString ActorData::getSelectedImage(DREmote t_emote)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, "emotions/selected.png");
  return l_texture;
}

QStringList ActorData::getOutfitNames()
{
  return {};
}

QVector<DREmote> ActorData::getEmotes()
{
  return {};
}

void ActorData::switchOutfit(QString t_outfit)
{
  mOutfit = t_outfit;
}

void ActorDataReader::loadActor(QString t_folder)
{
  mFolder = t_folder;
  ReadFromFile(AOApplication::getInstance()->get_character_path(t_folder, "char.json"));

  SetShowname(getStringValue("showname"));
  SetGender(getStringValue("gender"));
  SetSide(getStringValue("side"));

  loadOutfits();
}

QString ActorDataReader::getEmoteButton(DREmote t_emote, bool t_enabled)
{
  QString currentOutfit = GetOutfit();
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("outfits/" + currentOutfit + "/emotions/" + t_emote.comment + ".png").arg(t_emote.key));

  if(t_enabled) l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("outfits/" + currentOutfit + "/emotions/" + t_emote.comment + "_on.png"));

  return l_texture;
}

void ActorDataReader::loadOutfits()
{
  QString currentOutfit = GetOutfit();
  mOutfitNames.clear();
  QDir l_outfitsDirectory(AOApplication::getInstance()->get_character_folder_path(mFolder) + "/outfits");

  QStringList l_outfitSubDirectories = l_outfitsDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  if(l_outfitSubDirectories.length() <= 0) return;
  if(!l_outfitSubDirectories.contains("default")) currentOutfit = l_outfitSubDirectories[0];
  for (const QString &f_outfitPath : l_outfitSubDirectories)
  {

    QDir l_outfitPathFull(l_outfitsDirectory.filePath(f_outfitPath));

    if(!mOutfits.contains(f_outfitPath))
    {
      mOutfitNames.append(f_outfitPath);
      mOutfits[f_outfitPath] = new OutfitReader(mFolder, f_outfitPath);
    }

  }
}

QVector<DREmote> ActorDataReader::getEmotes()
{
  QString currentOutfit = GetOutfit();
  if(currentOutfit == "<All>")
  {
    QVector<DREmote> l_allEmotes = {};

    return l_allEmotes;
  }

  if(mOutfits.contains(currentOutfit))
  {
    return mOutfits[currentOutfit]->mEmotes;
  }
  return {};
}

QString ActorDataReader::getSelectedImage(DREmote t_emote)
{
  QString currentOutfit = GetOutfit();
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("outfits/" + currentOutfit + "/emotions/selected.png"));
  return l_texture;
}

QStringList ActorDataReader::getOutfitNames()
{
  return mOutfitNames;
}

void ActorDataReader::switchOutfit(QString t_outfit)
{
  QString currentOutfit = GetOutfit();
  if(mOutfitNames.contains(t_outfit) || t_outfit == "<All>")
  {
    ActorData::switchOutfit(t_outfit);
  }
}

QMap<QString, QRect> ActorDataReader::GetEmoteOverlays(QString outfit, QString emoteName)
{
  if(mOutfits.contains(outfit))
  {
    for(DREmote emote : mOutfits[outfit]->mEmotes)
    {
      if(emote.comment == emoteName)
      {
        return emote.emoteOverlays;
      }
    }
  }
  return {};
}


void LegacyActorReader::loadActor(QString folder)
{
  mFolder = folder;
  SetShowname(AOApplication::getInstance()->read_char_ini(mFolder, "options", "showname", mFolder).toString());
  SetSide(AOApplication::getInstance()->read_char_ini(mFolder, "options", "side", "wit").toString());
  SetGender("male");
}

QString LegacyActorReader::drLookupKey(const QStringList &keyList, const QString &targetKey)
{
  const QString finalTargetKey = targetKey.toLower();
  for (const QString &i_key : qAsConst(keyList))
    if (i_key.toLower() == finalTargetKey)
      return i_key;
  return targetKey;
}

QVector<DREmote> LegacyActorReader::getEmotes()
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
      l_emote.emoteName = l_emotions.at(Dialog);
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

QString LegacyActorReader::getEmoteButton(DREmote t_emote, bool t_enabled)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("emotions/button%1_off.png").arg(t_emote.key));

  if(t_enabled) l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString("emotions/button%1_on.png").arg(t_emote.key));
  return l_texture;
}

QString LegacyActorReader::getSelectedImage(DREmote t_emote)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, "emotions/selected.png");
  return l_texture;
}


OutfitReader::OutfitReader(QString t_character, QString t_outfit)
{
  mOutfitName = t_outfit;
  mCharacterName = t_character;
  mOutfitPath = AOApplication::getInstance()->get_character_folder_path(t_character) + "/outfits/" + t_outfit;

  if(!file_exists(mOutfitPath + "/outfit.json")) return;

  ReadFromFile(mOutfitPath  + "/outfit.json");

  QJsonArray overlayArray = getArrayValue("overlays");

  for(QJsonValueRef overlayData : overlayArray)
  {
    SetTargetObject(overlayData.toObject());
    QString overlayName = getStringValue("name");
    QRect overlayRect = getRectangleValue("rect");
    m_OverlayRectangles[overlayName] = overlayRect;


  }

  ReadSettings();
  ReadEmotes();
}

void OutfitReader::ReadSettings()
{
  ResetTargetObject();
  SetTargetObject("settings");
}

void OutfitReader::ReadEmotes()
{
  ResetTargetObject();
  QJsonArray l_emotesArray = getArrayValue("emotes");

  for(QJsonValueRef l_emoteData : l_emotesArray)
  {
    SetTargetObject(l_emoteData.toObject());
    QString l_emoteName = getStringValue("name");

    DREmote l_newEmote;

           //l_emote.key = i_key;
    l_newEmote.character = mCharacterName;

    l_newEmote.comment = l_emoteName;
    l_newEmote.anim = "";
    l_newEmote.outfitName = mOutfitName;
    l_newEmote.emoteName = l_emoteName;
    l_newEmote.dialog = "outfits/" + mOutfitName + "/" + l_emoteName;
    if(l_emoteData.toObject().contains("image")) l_newEmote.dialog = mOutfitName + "/" + getStringValue("image");
    l_newEmote.modifier = 0;
    l_newEmote.desk_modifier = true;

    SetTargetObject("overlays");

    for(QString overlayName : m_OverlayRectangles.keys())
    {
      QString overlayImage = getStringValue(overlayName);
      if(!overlayImage.trimmed().isEmpty())
      {
        l_newEmote.emoteOverlays[overlayImage] = m_OverlayRectangles[overlayName];
      }
    }

    mEmotes.append(l_newEmote);

  }
}
