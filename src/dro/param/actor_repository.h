#ifndef ACTOR_REPOSITORY_H
#define ACTOR_REPOSITORY_H

class QString;
class ActorData;

namespace dro::actor::repository
{
  ActorData *retrieve(QString t_folder);
}


namespace dro::actor::user
{
  ActorData *load(QString folder);
}


#endif // ACTOR_REPOSITORY_H
