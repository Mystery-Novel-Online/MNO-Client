#ifndef OUTFITREADER_H
#define OUTFITREADER_H
#include "modules/json/json_reader.h"

#include <datatypes.h>

class OutfitReader : public JSONReader
{
public:
  OutfitReader(QString t_character, QString t_outfit);

  void ReadSettings();
  void ReadEmotes();

  double getOutfitHeight();

  QVector<DREmote> mEmotes = {};
private:
  double mHeight = 0.1;
  QString mOutfitPath = "";
  QString mOutfitName = "";
  QString mCharacterName = "";

};



#endif // OUTFITREADER_H
