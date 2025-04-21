#ifndef LEGACYCHARACTERREADER_H
#define LEGACYCHARACTERREADER_H

#include "character_data_reader.h"



class LegacyCharacterReader : public CharacterDataReader
{
public:
  LegacyCharacterReader();

public:
  QString getShowname();
  QString getGender();
  QString getSide();
  QString drLookupKey(const QStringList &keyList, const QString &targetKey);
  QVector<DREmote> getEmotes();

  // CharacterData interface
public:
  QString getEmoteButton(DREmote t_emote, bool t_enabled);
  QString getSelectedImage(DREmote t_emote);
};

#endif // LEGACYCHARACTERREADER_H
