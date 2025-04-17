#ifndef ACTOR_LOADER_H
#define ACTOR_LOADER_H

#include <datatypes.h>
#include <modules/json/json_reader.h>

class ActorData
{
public:
  ActorData(){};

  virtual QString getEmoteSprite(DREmote t_emote);
  virtual QString getEmoteButton(DREmote t_emote, bool t_enabled);
  virtual QString getSelectedImage(DREmote t_emote);

  virtual QString GetShowname() {return mShowname;};
  virtual void SetShowname(QString showname) { mShowname = showname;};

  virtual QString GetGender() {return mGender;};
  virtual void SetGender(QString gender) { mGender = gender;};

  virtual QString GetSide() { return mSide; };
  virtual void SetSide(QString side) {mSide = side;};

  virtual QString GetOutfit() { return mOutfit; };
  virtual void SetOutfit(QString outfit) {mOutfit = outfit;};

  virtual QStringList getOutfitNames();
  virtual QVector<DREmote> getEmotes();

  virtual void loadActor(QString folder) = 0;
  virtual void switchOutfit(QString t_outfit);

  virtual QMap<QString, QRect> GetEmoteOverlays(QString outfit, QString emoteName) = 0;

  QString mFolder = "Makoto Naegi (DRO)";
private:
  QString mOutfit = "default";
  QString mShowname = "Makoto Naegi";
  QString mGender = "male";
  QString mSide = "wit";
};

class OutfitReader : public JSONReader
{
public:
  OutfitReader(QString t_character, QString t_outfit);

  void ReadSettings();
  void ReadEmotes();

  QVector<DREmote> mEmotes = {};
  QMap<QString, QRect> m_OverlayRectangles = {};

private:
  QString mOutfitPath = "";
  QString mOutfitName = "";
  QString mCharacterName = "";

  bool m_RuleDesk = true;

};

class ActorDataReader : public ActorData, public JSONReader
{
public:
  ActorDataReader() {};
  virtual void loadActor(QString folder) override;
  virtual QString getEmoteSprite(DREmote t_emote) override;
  virtual QString getEmoteButton(DREmote t_emote, bool t_enabled) override;
  virtual QString getSelectedImage(DREmote t_emote) override;
  virtual QStringList getOutfitNames() override;
  virtual void switchOutfit(QString t_outfit) override;

  virtual QMap<QString, QRect> GetEmoteOverlays(QString outfit, QString emoteName) override;

private:
  void loadOutfits();
  QMap<QString, OutfitReader*> mOutfits = {};
  QStringList mOutfitNames = {};
  QStringList mOutfitsOrder = {};

  virtual QVector<DREmote> getEmotes() override;


};



class LegacyActorReader : public ActorDataReader
{
public:
  LegacyActorReader(){};

public:
  virtual void loadActor(QString folder) override;

  QString drLookupKey(const QStringList &keyList, const QString &targetKey);

  virtual QVector<DREmote> getEmotes() override;
  virtual QString getEmoteSprite(DREmote t_emote) override;
  virtual QString getEmoteButton(DREmote t_emote, bool t_enabled) override;
  virtual QString getSelectedImage(DREmote t_emote) override;
  virtual QMap<QString, QRect> GetEmoteOverlays(QString outfit, QString emoteName) override { return {}; };

};


class ActorLoader
{
public:
  static ActorData *GetCharacter(QString folder);
};





#endif // ACTOR_LOADER_H
