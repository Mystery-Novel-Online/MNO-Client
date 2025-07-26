#ifndef ACTOR_LOADER_H
#define ACTOR_LOADER_H

#include "engine/param/json_reader.h"
#include "mk2/spriteplayer.h"
#include <rolechat/actor/ActorOutfit.h>
#include <rolechat/actor/IActorData.h>

class LegacyActorReader : public rolechat::actor::IActorData
{
public:
  LegacyActorReader() = default;

  void load(const std::string& folder, const std::string& path) override;

  std::vector<ActorEmote> emotes() override;
  std::string buttonImage(const ActorEmote& t_emote, bool t_enabled) const override;
  std::string selectedImage(const ActorEmote& t_emote) const override;

  QString DRLookupKey(const QStringList &keyList, const QString &targetKey);
};

#endif // ACTOR_LOADER_H
