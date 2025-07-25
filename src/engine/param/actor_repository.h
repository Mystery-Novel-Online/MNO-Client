#ifndef ACTOR_REPOSITORY_H
#define ACTOR_REPOSITORY_H

class QString;
class QStringList;
class ActorData;

#include <rolechat/actor/IActorData.h>

namespace engine::actor::repository
{
  rolechat::actor::IActorData *retrieve(QString t_folder);
}


namespace engine::actor::user
{
  rolechat::actor::IActorData *switchCharacter(QString folder);
  rolechat::actor::IActorData *load(QString folder);
  rolechat::actor::IActorData *retrieve();
  QString name();
  void toggleLayer(const std::string& name, bool state);
  bool layerState(const std::string& name);
  void setOutfitList(QStringList outfits);
}


#endif // ACTOR_REPOSITORY_H
