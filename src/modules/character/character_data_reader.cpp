#include "aoapplication.h"
#include "character_data_reader.h"

#include <QDir>

CharacterDataReader::CharacterDataReader()
{

}

void CharacterDataReader::loadCharacter(QString t_folder)
{
  mFolder = t_folder;
  ReadFromFile(AOApplication::getInstance()->get_character_path(t_folder, "char.json"));

  mShowname = getStringValue("showname");
  mGender = getStringValue("gender");
  mSide = getStringValue("side");
  mHeight = getDoubleValue("height");

  loadOutfits();
}

QString CharacterDataReader::getEmoteButton(DREmote t_emote, bool t_enabled)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString(mOutfit + "/emotions/" + t_emote.comment + ".png").arg(t_emote.key));

  if(t_enabled) l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString(mOutfit + "/emotions/" + t_emote.comment + "_on.png"));

  return l_texture;
}

void CharacterDataReader::loadOutfits()
{
  mOutfitNames.clear();
  QDir l_outfitsDirectory(AOApplication::getInstance()->get_character_folder_path(mFolder));

  QStringList l_outfitSubDirectories = l_outfitsDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  if(l_outfitSubDirectories.length() <= 0) return;
  if(!l_outfitSubDirectories.contains("default")) mOutfit = l_outfitSubDirectories[0];
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

QVector<DREmote> CharacterDataReader::getEmotes()
{
  if(mOutfit == "<All>")
  {
    QVector<DREmote> l_allEmotes = {};

    return l_allEmotes;
  }

  if(mOutfits.contains(mOutfit))
  {
    return mOutfits[mOutfit]->mEmotes;
  }
  return {};
}

QString CharacterDataReader::getSelectedImage(DREmote t_emote)
{
  QString l_texture = AOApplication::getInstance()->get_character_path(t_emote.character, QString(mOutfit + "/emotions/selected.png"));
  return l_texture;
}

QStringList CharacterDataReader::getOutfitNames()
{
  return mOutfitNames;
}

void CharacterDataReader::switchOutfit(QString t_outfit)
{
  if(mOutfitNames.contains(t_outfit) || t_outfit == "<All>") mOutfit = t_outfit;
}

double CharacterDataReader::getHeight()
{
  return mHeight;
}
