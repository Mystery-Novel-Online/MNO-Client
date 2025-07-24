#ifndef ACTOR_REPOSITORY_H
#define ACTOR_REPOSITORY_H

class QString;
class QStringList;
class ActorData;

namespace dro::actor::repository
{
  ActorData *retrieve(QString t_folder);
}


namespace dro::actor::user
{
  ActorData *switchCharacter(QString folder);
  ActorData *load(QString folder);
  ActorData *retrieve();
  QString name();
  void toggleLayer(const QString& name, bool state);
  bool layerState(const QString& name);
  void setOutfitList(QStringList outfits);
}


#endif // ACTOR_REPOSITORY_H
