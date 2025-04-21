#include "outfit_reader.h"
#include "aoapplication.h"
#include "file_functions.h"

OutfitReader::OutfitReader(QString t_character, QString t_outfit)
{
  mOutfitName = t_outfit;
  mCharacterName = t_character;
  mOutfitPath = AOApplication::getInstance()->get_character_folder_path(t_character) + "/" + t_outfit;

  if(!file_exists(mOutfitPath + "/outfit.json")) return;

  ReadFromFile(mOutfitPath  + "/outfit.json");

  ReadSettings();
  ReadEmotes();
}

void OutfitReader::ReadSettings()
{
  SetTargetObject("settings");
  mHeight = getDoubleValue("height");
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
    l_newEmote.dialog = mOutfitName + "/" + l_emoteName;
    if(l_emoteData.toObject().contains("image")) l_newEmote.dialog = mOutfitName + "/" + getStringValue("image");
    l_newEmote.modifier = 0;
    l_newEmote.desk_modifier = true;

    mEmotes.append(l_newEmote);

  }
}

double OutfitReader::getOutfitHeight()
{
  return mHeight;
}
