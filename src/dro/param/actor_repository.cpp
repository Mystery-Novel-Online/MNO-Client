#include "actor_repository.h"
#include <QMap>
#include <QString>
#include "aoapplication.h"
#include "dro/fs/fs_reading.h"
#include "dro/param/actor/actor_loader.h"

QMap<QString, bool> s_layersEnabled = {};
ActorData* s_currentActor = nullptr;

bool dro::actor::user::layerState(const QString &name)
{
  if(s_layersEnabled.contains(name)) return s_layersEnabled[name];
  return true;
}

void dro::actor::user::toggleLayer(const QString &name, bool state)
{
  s_layersEnabled[name] = state;
}

ActorData *dro::actor::user::load(QString folder)
{
  static QString currentFolder = "<NOCHAR>";
  s_layersEnabled.clear();
  if(folder == currentFolder)
  {
    if(s_currentActor != nullptr) s_currentActor->reload();
    return s_currentActor;
  }

  QString l_jsonPath = AOApplication::getInstance()->get_character_path(folder, "char.json");

  if(FS::Checks::FileExists(l_jsonPath))
  {
    s_currentActor = new ActorDataReader();
    s_currentActor->LoadActor(folder);
  }
  else
  {
    s_currentActor = new LegacyActorReader();
    s_currentActor->LoadActor(folder);
  }

  return s_currentActor;
}

ActorData *dro::actor::user::retrieve()
{
  return s_currentActor;
}
