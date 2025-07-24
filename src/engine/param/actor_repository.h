#ifndef ACTOR_REPOSITORY_H
#define ACTOR_REPOSITORY_H

class QString;
class QStringList;
class ActorData;

namespace engine::actor::repository
{
  ActorData *retrieve(QString t_folder);
}


namespace engine::actor::user
{
  ActorData *switchCharacter(QString folder);
  ActorData *load(QString folder);
  ActorData *retrieve();
  QString name();
  void toggleLayer(const std::string& name, bool state);
  bool layerState(const std::string& name);
  void setOutfitList(QStringList outfits);
}


#endif // ACTOR_REPOSITORY_H
