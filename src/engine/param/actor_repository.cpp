#include "actor_repository.h"

#include "modules/theme/thememanager.h"
#include "engine/fs/fs_reading.h"
#include "engine/param/actor/actor_loader.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/system/theme_scripting.h"
#include "engine/fs/fs_characters.h"

QMap<QString, bool> s_layersEnabled = {};
ActorData* s_currentActor = nullptr;
static QString s_currentFolder = "<NOCHAR>";

bool engine::actor::user::layerState(const std::string &name)
{
  QString qName = QString::fromStdString(name);
  if(s_layersEnabled.contains(qName)) return s_layersEnabled[qName];
  return true;
}

void engine::actor::user::toggleLayer(const std::string &name, bool state)
{
  QString qName = QString::fromStdString(name);
  s_layersEnabled[qName] = state;
}

ActorData *engine::actor::user::load(QString folder)
{
  s_layersEnabled.clear();
  if(folder == s_currentFolder)
  {
    if(s_currentActor != nullptr) s_currentActor->reload();
    return s_currentActor;
  }

  s_currentFolder = folder;
  QString l_jsonPath = fs::characters::getFilePath(folder, "char.json");

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

ActorData *engine::actor::user::retrieve()
{
  return s_currentActor;
}

QString engine::actor::user::name()
{
  return s_currentFolder;
}

ActorData *engine::actor::repository::retrieve(QString t_folder)
{
  static QMap<QString, QPair<QDateTime, ActorData*>> s_cache;

  QString l_jsonPath = fs::characters::getFilePath(t_folder, "char.json");

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

ActorData *engine::actor::user::switchCharacter(QString folder)
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

void engine::actor::user::setOutfitList(QStringList outfits)
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
