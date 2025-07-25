#include "actor_repository.h"

#include "modules/theme/thememanager.h"
#include "engine/fs/fs_reading.h"
#include "engine/param/actor/actor_loader.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/system/theme_scripting.h"
#include "engine/fs/fs_characters.h"
#include <rolechat/actor/JsonActorData.h>

QMap<QString, bool> s_layersEnabled = {};
rolechat::actor::IActorData* s_currentActor = nullptr;
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

rolechat::actor::IActorData *engine::actor::user::load(QString folder)
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
    s_currentActor = new rolechat::actor::JsonActorData();
    s_currentActor->load(folder.toStdString(), fs::characters::getDirectoryPath(folder).toStdString());
  }
  else
  {
    s_currentActor = new LegacyActorReader();
    s_currentActor->load(folder.toStdString(), fs::characters::getDirectoryPath(folder).toStdString());
  }

  return s_currentActor;
}

rolechat::actor::IActorData *engine::actor::user::retrieve()
{
  return s_currentActor;
}

QString engine::actor::user::name()
{
  return s_currentFolder;
}

rolechat::actor::IActorData *engine::actor::repository::retrieve(QString t_folder)
{
  static QMap<QString, QPair<QDateTime, rolechat::actor::IActorData*>> s_cache;

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

    rolechat::actor::IActorData* l_returnData = new rolechat::actor::JsonActorData();
    l_returnData->load(t_folder.toStdString(), fs::characters::getDirectoryPath(t_folder).toStdString());
    s_cache[t_folder] = qMakePair(lastModified, l_returnData);
    return l_returnData;
  }

  rolechat::actor::IActorData *l_returnData = new LegacyActorReader();
  l_returnData->load(t_folder.toStdString(), fs::characters::getDirectoryPath(t_folder).toStdString());
  return l_returnData;
}

rolechat::actor::IActorData *engine::actor::user::switchCharacter(QString folder)
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

  rolechat::actor::IActorData* actor = load(folder);
  if(actor != nullptr)
  {
    const std::vector<std::string>& outfits = actor->outfitNames();
    QStringList l_charaOutfits;
    for (const std::string& outfit : outfits)
    {
      l_charaOutfits << QString::fromStdString(outfit);
    }
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
