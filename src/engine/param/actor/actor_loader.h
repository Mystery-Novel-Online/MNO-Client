#ifndef ACTOR_LOADER_H
#define ACTOR_LOADER_H

#include "engine/param/json_reader.h"
#include "mk2/spriteplayer.h"
#include <rolechat/actor/ActorOutfit.h>

struct ActorScalingPreset
{
  QString name = "N/A";
  int verticalAlign = 0;
  int scale = 1000;
};

class ActorData
{
public:
  ActorData(){};
  virtual ~ActorData() = default;

  virtual QString GetEmoteSprite(const ActorEmote& t_emote);
  virtual QString GetEmoteButton(const ActorEmote& t_emote, bool t_enabled);
  virtual QString GetSelectedImage(const ActorEmote& t_emote);

  virtual void SetShowname(const QString& showname) { m_Showname = showname;};
  virtual QString GetShowname() { return m_Showname.isEmpty() ? m_Folder : m_Showname;};

  virtual void SetGender(const QString& gender) { m_Gender = gender;};
  virtual QString GetGender() {return m_Gender;};

  virtual void SetSide(const QString& side) {m_Side = side;};
  virtual QString GetSide() { return m_Side; };

  virtual void SetOutfit(const QString& outfit) {m_CurrentOutfit = outfit;};
  virtual QString GetOutfit() { return m_CurrentOutfit; };

  virtual void SetScalingMode(const QString& mode) {m_ScalingMode = mode;};
  virtual mk2::SpritePlayer::ScalingMode GetScalingMode()
  {
    const static QMap<QString, mk2::SpritePlayer::ScalingMode> scalingModeMap =
    {
      {"width_smooth", mk2::SpritePlayer::WidthSmoothScaling},
      {"width_pixels", mk2::SpritePlayer::WidthPixelScaling},
    };

    if(scalingModeMap.contains(m_ScalingMode.toLower())) return scalingModeMap[m_ScalingMode.toLower()];
    return mk2::SpritePlayer::AutomaticScaling;
  };

  virtual void SetScalingPresets(QVector<ActorScalingPreset> presets) {m_Presets = presets;};
  virtual QVector<ActorScalingPreset> GetScalingPresets() { return m_Presets; };

  virtual QStringList GetOutfitNames();
  virtual QVector<ActorEmote> GetEmotes();

  virtual void SetFolder(const QString& folder) { m_Folder = folder; };
  virtual QString GetFolder() { return m_Folder; };

  virtual void LoadActor(const QString& folder) = 0;
  virtual void SwitchOutfit(const QString& t_outfit);

  virtual void reload() {};

  virtual QVector<ActorLayer> GetEmoteOverlays(const QString& outfit, const QString& emoteName) = 0;
  virtual rolechat::actor::ActorOutfit* GetEmoteOutfit(const QString& emoteName) = 0;

private:
  QVector<ActorScalingPreset> m_Presets = {};
  QString m_Folder = "Makoto Naegi (DRO)";
  QString m_Showname = "Makoto Naegi";
  QString m_Gender = "male";
  QString m_Side = "wit";
  QString m_CurrentOutfit = "default";
  QString m_ScalingMode = "automatic";
};

class ActorDataReader : public ActorData, public JSONReader
{
public:
  ActorDataReader() = default;

  void LoadActor(const QString& folder) override;
  QString GetEmoteSprite(const ActorEmote& t_emote) override;
  QString GetEmoteButton(const ActorEmote& t_emote, bool t_enabled) override;
  QString GetSelectedImage(const ActorEmote& t_emote) override;
  QStringList GetOutfitNames() override;
  void SwitchOutfit(const QString& t_outfit) override;
  QString GetShowname() override;

  QVector<ActorEmote> GetEmotes() override;
  QVector<ActorLayer> GetEmoteOverlays(const QString& outfit, const QString& emoteName) override;
  rolechat::actor::ActorOutfit* GetEmoteOutfit(const QString& emoteName) override;

  void reload() override;

private:
  void LoadOutfits();

  QString m_Showname = "";
  QMap<QString, rolechat::actor::ActorOutfit*> m_Outfits = {};
  QStringList m_OutfitNames = {};
  QStringList m_OutfitsOrder = {};

  QMap<QString, QDateTime> m_OutfitModifiedTimes;
};


class LegacyActorReader : public ActorDataReader
{
public:
  LegacyActorReader() = default;

  void LoadActor(const QString& folder) override;

  QString DRLookupKey(const QStringList &keyList, const QString &targetKey);

  QVector<ActorEmote> GetEmotes() override;
  QString GetEmoteSprite(const ActorEmote& emote) override { return ""; };
  QString GetEmoteButton(const ActorEmote& t_emote, bool t_enabled) override;
  QString GetSelectedImage(const ActorEmote& t_emote) override;
  QVector<ActorLayer> GetEmoteOverlays(const QString& outfit, const QString& emoteName) override { return {}; };
  rolechat::actor::ActorOutfit* GetEmoteOutfit(const QString& emoteName) override { return nullptr; };

};

class ActorLoader
{
public:
  static ActorData *GetCharacter(const QString& folder);
};

#endif // ACTOR_LOADER_H
