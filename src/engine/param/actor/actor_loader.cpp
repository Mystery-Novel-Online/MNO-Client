#include "actor_loader.h"
#include <utils.h>
#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_characters.h"
#include <rolechat/actor/ActorOutfit.h>

static QMap<QString, ActorData*> s_CachedCharacters;

ActorData *ActorLoader::GetCharacter(const QString& folder)
{
  if(!s_CachedCharacters.contains(folder))
  {
    QString charaConfigPath = fs::characters::getFilePath(folder, "char.json");
    ActorData *characterData;
    if(FS::Checks::FileExists(charaConfigPath))
    {
      characterData = new ActorDataReader();
    }
    else
    {
      characterData = new LegacyActorReader();
    }

    characterData->LoadActor(folder);
    s_CachedCharacters[folder] = characterData;
  }

  return s_CachedCharacters[folder];
}

QString ActorData::GetEmoteSprite(const ActorEmote& emote)
{
  Q_UNUSED(emote);
  return "";
}

QString ActorData::GetEmoteButton(const ActorEmote& emote, bool enabled)
{
  const QString filename = enabled ? QString("emotions/button%1_on.png").arg(QString::fromStdString(emote.key)) : QString("emotions/button%1_off.png").arg(QString::fromStdString(emote.key));
  return fs::characters::getFilePath(QString::fromStdString(emote.character), filename);
}

QString ActorData::GetSelectedImage(const ActorEmote& emote)
{
  return fs::characters::getFilePath(QString::fromStdString(emote.character), "emotions/selected.png");
}

QStringList ActorData::GetOutfitNames()
{
  return {};
}

QVector<ActorEmote> ActorData::GetEmotes()
{
  return {};
}

void ActorData::SwitchOutfit(const QString& t_outfit)
{
  m_CurrentOutfit = t_outfit;
}

void ActorDataReader::LoadActor(const QString& folder)
{
  SetFolder(folder);
  ReadFromFile(fs::characters::getFilePath(folder, "char.json"));

  SetShowname(getStringValue("showname"));
  SetGender(getStringValue("gender"));
  SetSide(getStringValue("side"));

  if(isValueExists("scaling_mode"))
  {
    const QString scalingMode = getStringValue("scaling_mode");
    SetScalingMode(scalingMode);
  }

  m_OutfitsOrder = getStringArrayValue("outfit_order");

  QVector<ActorScalingPreset> presets = {};
  for(QJsonValueRef presetValue : getArrayValue("scaling_presets"))
  {
    SetTargetObject(presetValue.toObject());
    if(isValueExists("name"))
    {
      ActorScalingPreset preset;
      preset.name = getStringValue("name");
      preset.verticalAlign = getIntValue("vertical");
      if(isValueExists("scale"))
        preset.scale = getIntValue("scale");
      presets.append(preset);
    }
  }
  SetScalingPresets(presets);

  LoadOutfits();
}

QString ActorDataReader::GetEmoteSprite(const ActorEmote& emote)
{
  Q_UNUSED(emote);
  return "";
}

QString ActorDataReader::GetEmoteButton(const ActorEmote& emote, bool enabled)
{
  const QString path = QString("outfits/%1/emotions/%2%3.png").arg(QString::fromStdString(emote.outfitName), QString::fromStdString(emote.comment), enabled ? "_on" : "");
  return fs::characters::getFilePath(QString::fromStdString(emote.character), path);
}

QString ActorDataReader::GetSelectedImage(const ActorEmote& emote)
{
  QString currentOutfit = GetOutfit();
  QString path  = QString("outfits/%1/emotions/selected.png").arg(currentOutfit);
  return fs::characters::getFilePath(QString::fromStdString(emote.character), path);
}

QStringList ActorDataReader::GetOutfitNames()
{
  return m_OutfitNames;
}

void ActorDataReader::SwitchOutfit(const QString& outfit)
{
  if(m_OutfitNames.contains(outfit) || outfit == "<All>")
    ActorData::SwitchOutfit(outfit);
}

QString ActorDataReader::GetShowname()
{
  const QString currentOutfit = GetOutfit();
  if(!m_Outfits.contains(currentOutfit)) return ActorData::GetShowname();
  const QString outfitShowname = QString::fromStdString(m_Outfits[currentOutfit]->showname());
  return outfitShowname.isEmpty() ? ActorData::GetShowname() : outfitShowname;
}

void ActorDataReader::LoadOutfits()
{
  m_OutfitNames.clear();

  const QString actorPath = fs::characters::getDirectoryPath(GetFolder());
  const QString outfitPath = actorPath + "/outfits";
  QDir outfitDir(outfitPath);

  QStringList subdirs = outfitDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
  if (subdirs.isEmpty()) return;

  for (const QString &name : subdirs)
  {

    QString fullOutfitPath = outfitPath + "/" + name + "/outfit.json";
    QFileInfo outfitInfo(fullOutfitPath);
    QDateTime modifiedTime = outfitInfo.lastModified();

    bool needsReload = true;

    if (m_Outfits.contains(name) && m_OutfitModifiedTimes.contains(name))
    {
      if (m_OutfitModifiedTimes[name] == modifiedTime)
      {
        needsReload = false;
      }
      else
      {
        delete m_Outfits[name];
        m_Outfits.remove(name);
      }
    }

    if (needsReload)
    {
      m_OutfitNames.append(name);
      m_Outfits[name] = new rolechat::actor::ActorOutfit(GetFolder().toStdString(), name.toStdString(), actorPath.toStdString());
      m_OutfitModifiedTimes[name] = modifiedTime;
    }
    else
    {
      m_OutfitNames.append(name);  // Still include in name list
    }
  }


  QStringList ordered;
  for (const QString &name : m_OutfitsOrder)
    if (m_OutfitNames.contains(name)) ordered.append(name);

  for (const QString& name : m_OutfitNames)
    if (!m_OutfitsOrder.contains(name))
      ordered.append(name);

  m_OutfitNames = ordered;

}


void ActorDataReader::reload()
{
  LoadOutfits();
}


QVector<ActorEmote> ActorDataReader::GetEmotes()
{
  QString currentOutfit = GetOutfit();

  if(currentOutfit == "<All>")
  {
    QVector<ActorEmote> all = {};
    for(QString &outfit : m_OutfitNames)
      if(m_Outfits.contains(outfit)) all.append(QVector<ActorEmote>::fromStdVector(m_Outfits[outfit]->emotes()));

    return all;
  }

  return m_Outfits.contains(currentOutfit) ? QVector<ActorEmote>::fromStdVector(m_Outfits[currentOutfit]->emotes()) : QVector<ActorEmote>();
}

QVector<ActorLayer> ActorDataReader::GetEmoteOverlays(const QString& outfit, const QString& emoteName)
{
  //if(m_Outfits.contains(outfit))
  //{
  //  for (const ActorEmote& emote : m_Outfits[outfit]->m_Emotes)
  //    if (emote.comment == emoteName) return emote.emoteOverlays;
  //}
  return {};
}

rolechat::actor::ActorOutfit *ActorDataReader::GetEmoteOutfit(const QString& emotePath)
{
  if (!emotePath.contains("outfits/"))
    return nullptr;

  const QStringList parts = emotePath.split('/');
  if (parts.size() < 3) return nullptr;

  const QString& outfit = parts[1];
  return m_Outfits.value(outfit, nullptr);
}

void LegacyActorReader::LoadActor(const QString& folder)
{
  SetFolder(folder);
  auto* app = AOApplication::getInstance();
  SetShowname(app->read_char_ini(folder, "options", "showname", folder).toString());
  SetSide(app->read_char_ini(folder, "options", "side", "wit").toString());
  SetGender("male");
}

QString LegacyActorReader::DRLookupKey(const QStringList &keys, const QString &target)
{
  const QString targetLower = target.toLower();
  for (const QString& key : keys)
    if (key.toLower() == targetLower)
      return key;
  return target;
}

QVector<ActorEmote> LegacyActorReader::GetEmotes()
{
  QVector<ActorEmote> r_emote_list;

  QStringList l_chr_list = AOApplication::getInstance()->get_char_include(GetFolder());
  l_chr_list.append(GetFolder());

#ifdef QT_DEBUG
  qDebug().noquote() << QString("Compiling char.ini for character <%1>").arg(GetFolder());
#endif

  for (const QString &i_chr : l_chr_list)
  {
    if (!FS::Checks::DirectoryExists(fs::characters::getDirectoryPath(i_chr)))
    {
      qWarning().noquote()
      << QString("Parent character <%1> not found, character <%2> cannot use it.").arg(i_chr, GetFolder());
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
      r_emote_list.append(l_emote);
    }
  }

  return r_emote_list;
}

QString LegacyActorReader::GetEmoteButton(const ActorEmote& t_emote, bool t_enabled)
{
  QString l_texture = fs::characters::getFilePath(QString::fromStdString(t_emote.character), QString("emotions/button%1_off.png").arg(QString::fromStdString(t_emote.key)));

  if(t_enabled) l_texture = fs::characters::getFilePath(QString::fromStdString(t_emote.character), QString("emotions/button%1_on.png").arg(QString::fromStdString(t_emote.key)));
  return l_texture;
}

QString LegacyActorReader::GetSelectedImage(const ActorEmote& t_emote)
{
  QString l_texture = fs::characters::getFilePath(QString::fromStdString(t_emote.character), "emotions/selected.png");
  return l_texture;
}
