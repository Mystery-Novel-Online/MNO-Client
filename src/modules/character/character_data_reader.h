#ifndef CHARACTERDATAREADER_H
#define CHARACTERDATAREADER_H

#include "character_data.h"
#include "modules/json/json_reader.h"
#include "outfit_reader.h"

class CharacterDataReader : public CharacterData, public JSONReader
{
public:
  CharacterDataReader();
  void loadCharacter(QString t_folder);

  QString getEmoteButton(DREmote t_emote, bool t_enabled);
  QString getSelectedImage(DREmote t_emote);
  QStringList getOutfitNames();
  void switchOutfit(QString t_outfit);
  double getHeight();

private:
  void loadOutfits();
  QMap<QString, OutfitReader*> mOutfits = {};
  QStringList mOutfitNames = {};
  double mHeight = 0;

  QVector<DREmote> getEmotes();


};

#endif // CHARACTERDATAREADER_H
