#include "actor_repository.h"

#include "modules/theme/thememanager.h"
#include "dro/fs/fs_reading.h"
#include "dro/param/actor/actor_loader.h"
#include "dro/interface/courtroom_layout.h"
#include "dro/system/theme_scripting.h"

QMap<QString, bool> s_layersEnabled = {};
ActorData* s_currentActor = nullptr;
static QString s_currentFolder = "<NOCHAR>";

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
  s_layersEnabled.clear();
  if(folder == s_currentFolder)
  {
    if(s_currentActor != nullptr) s_currentActor->reload();
    return s_currentActor;
  }

  s_currentFolder = folder;
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

QString dro::actor::user::name()
{
  return s_currentFolder;
}

ActorData *dro::actor::repository::retrieve(QString t_folder)
{
  static QMap<QString, QPair<QDateTime, ActorData*>> s_cache;

  QString l_jsonPath = AOApplication::getInstance()->get_character_path(t_folder, "char.json");

  if(FS::Checks::FileExists(l_jsonPath))
  {
    QFileInfo fileInfo(l_jsonPath);
    QDateTime lastModified = fileInfo.lastModified();

    if (s_cache.contains(t_folder)) {
      const auto& cached = s_cache[t_folder];
      if (cached.first == lastModified)
      {
        cached.second->reload();
        return cached.second;
      } else
      {
        delete cached.second;
      }
    }

    ActorData* l_returnData = new ActorDataReader();
    l_returnData->LoadActor(t_folder);
    s_cache[t_folder] = qMakePair(lastModified, l_returnData);
    return l_returnData;
  }

  ActorData *l_returnData = new LegacyActorReader();
  l_returnData->LoadActor(t_folder);
  return l_returnData;
}

ActorData *dro::actor::user::switchCharacter(QString folder)
{

  QStringList animations = {"None"};

  QFile characterAnimations(FS::Paths::FindFile("characters/" + folder + "/animations.ini"));
  if (characterAnimations.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&characterAnimations);
    while (!in.atEnd())
    {
      QString line = in.readLine().trimmed();
      if (!line.isEmpty()) animations.append(line);
    }
    characterAnimations.close();
  }
  else
  {
    animations.append(FS::Paths::GetFileList("characters/" + folder + "/animations", true, "json"));
  }

  for(const QString &animationIniPath : FS::Paths::FindFiles("configs/animations.ini"))
  {
    QFile file(animationIniPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&file);
      while (!in.atEnd())
      {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) animations.append(line);
      }
      file.close();
    }
  }


  courtroom::lists::setAnimations(animations);
  LuaBridge::LuaEventCall("OnCharacterLoad", folder.toStdString());
  QStringList l_OutfitNames = {"<All>"};

  ActorData* actor = load(folder);
  if(actor != nullptr)
  {
    QStringList l_charaOutfits = actor->GetOutfitNames();
    l_OutfitNames.append(l_charaOutfits);
  }
  setOutfitList(l_OutfitNames);

  return retrieve();
}

void dro::actor::user::setOutfitList(QStringList outfits)
{
  QWidget *l_outfitSelectorWidget = ThemeManager::get().getWidget("outfit_selector");

  if (dynamic_cast<QComboBox*>(l_outfitSelectorWidget) != nullptr)
  {
    QComboBox* l_outfitSelectorCombo = dynamic_cast<QComboBox*>(l_outfitSelectorWidget);
    l_outfitSelectorCombo->clear();
    l_outfitSelectorCombo->addItems(outfits);
    if(l_outfitSelectorCombo->count() > 1) l_outfitSelectorCombo->setCurrentIndex(1);
  }
}
