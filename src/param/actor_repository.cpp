#include "actor_repository.h"

#include "system/user_database.h"
#include "modules/theme/legacythememanager.h"
#include "fs/fs_reading.h"
#include "param/actor/actor_loader.h"
#include "interface/courtroom_layout.h"
#include "system/theme_scripting.h"
#include "fs/fs_characters.h"
#include <rolechat/actor/JsonActorData.h>

QMap<QString, bool> s_layersEnabled = {};
std::unique_ptr<rolechat::actor::IActorData> s_currentActor = nullptr;

std::unordered_map<std::string, std::unordered_map<QString, QDateTime>> s_fileTimes;
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
  isModified(folder.toStdString());
  if(folder == s_currentFolder)
  {
    if(s_currentActor != nullptr) s_currentActor->reload();
    return s_currentActor.get();
  }

  GetDB().incrementCharacterUsage(folder.toStdString());

  s_currentFolder = folder;
  QString l_jsonPath = fs::characters::getFilePath(folder, "char.json");

  if(FS::Checks::FileExists(l_jsonPath))
  {
    auto actor = std::make_unique<rolechat::actor::JsonActorData>();

    actor = std::make_unique<rolechat::actor::JsonActorData>();
    actor->load(folder.toStdString(), fs::characters::getDirectoryPath(folder).toStdString());

    auto outfitNames = actor->outfitNames();
    auto outfits = actor->outfits();

    for(auto outfit : outfitNames)
    {
      for(auto layer : outfits[outfit]->layers()){
        if(!layer.toggleName.empty())
          toggleLayer(layer.toggleName, !layer.defaultDisabled);
      }
    }

    s_currentActor = std::move(actor);
  }
  else
  {
    auto actor = std::make_unique<LegacyActorReader>();
    actor->load(folder.toStdString(), fs::characters::getDirectoryPath(folder).toStdString());
    s_currentActor = std::move(actor);
  }

  return s_currentActor.get();
}

rolechat::actor::IActorData *engine::actor::user::retrieve()
{
  return s_currentActor.get();
}

QString engine::actor::user::name()
{
  return s_currentFolder;
}

rolechat::actor::IActorData* engine::actor::repository::retrieve(const QString& folder)
{
  static QMap<QString, CachedActor> actorCache;

  const QString jsonPath = fs::characters::getFilePath(folder, "char.json");

  if(FS::Checks::FileExists(jsonPath))
  {
    QFileInfo fileInfo(jsonPath);
    QDateTime lastModified = fileInfo.lastModified();

    auto it = actorCache.find(folder);

    if(it != actorCache.end())
    {
      if(it->lastModified == lastModified)
        return it->data.get();

      it->data.reset();
      actorCache.erase(it);
    }

    auto actor = std::make_shared<rolechat::actor::JsonActorData>();
    actor->load(folder.toStdString(), fs::characters::getDirectoryPath(folder).toStdString());
    actorCache[folder] = { lastModified, std::move(actor) };
    return actorCache[folder].data.get();
  }

  auto it = actorCache.find(folder);
  if(it == actorCache.end())
  {
    auto actor = std::make_shared<LegacyActorReader>();
    actor->load(folder.toStdString(), fs::characters::getDirectoryPath(folder).toStdString());
    actorCache[folder] = { {}, std::move(actor) };
  }
  return actorCache[folder].data.get();
}

rolechat::actor::IActorData *engine::actor::user::switchCharacter(QString folder)
{

  QStringList animations = {"None"};

  QString characterPath = engine::fs::characters::getDirectoryPath(folder);

  QFile characterAnimations(characterPath + "/animations.ini");
  if(characterAnimations.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&characterAnimations);
    while(!in.atEnd())
    {
      QString line = in.readLine().trimmed();
      if(!line.isEmpty()) animations.append(line);
    }
    characterAnimations.close();
  }
  else
  {
    animations.append(FS::Paths::GetFileList(characterPath + "/animations", true, "json"));
  }

  for(const QString &animationIniPath : FS::Paths::FindFiles("configs/animations.ini"))
  {
    QFile file(animationIniPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QTextStream in(&file);
      while(!in.atEnd())
      {
        QString line = in.readLine().trimmed();
        if(!line.isEmpty()) animations.append(line);
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
    for(const std::string& outfit : outfits)
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
  QWidget *l_outfitSelectorWidget = LegacyThemeManager::get().getWidget("outfit_selector");

  if(dynamic_cast<QComboBox*>(l_outfitSelectorWidget) != nullptr)
  {
    QComboBox* l_outfitSelectorCombo = dynamic_cast<QComboBox*>(l_outfitSelectorWidget);
    l_outfitSelectorCombo->clear();
    l_outfitSelectorCombo->addItems(outfits);
    if(l_outfitSelectorCombo->count() > 1) l_outfitSelectorCombo->setCurrentIndex(1);
  }
}

bool engine::actor::user::isModified(const std::string &name)
{
  const QString folderPath = engine::fs::characters::getDirectoryPath(QString::fromStdString(name));

  if(folderPath.isEmpty())
    return false;

  QDir root(folderPath);
  if(!root.exists())
    return false;

  std::vector<QFileInfo> jsonFiles;
  QList<QDir> stack;
  stack.append(root);

  while(!stack.isEmpty())
  {
    QDir dir = stack.takeLast();

    QFileInfoList files = dir.entryInfoList(
        {"*.json"},
        QDir::Files | QDir::Readable
        );

    for(const QFileInfo &info : files)
      jsonFiles.push_back(info);

    QFileInfoList subdirs = dir.entryInfoList(
        QDir::Dirs | QDir::NoDotAndDotDot
        );

    for(const QFileInfo &sd : subdirs)
      stack.append(QDir(sd.absoluteFilePath()));
  }

  auto &prevState = s_fileTimes[name];
  bool modified = false;

  std::unordered_map<QString, QDateTime> newState;

  for(const QFileInfo &info : jsonFiles)
  {
    QString path = info.absoluteFilePath();
    QDateTime lastMod = info.lastModified();
    newState[path] = lastMod;

    auto it = prevState.find(path);

    if(it == prevState.end())
    {
      modified = true;
    }
    else
    {
      if(lastMod > it->second)
        modified = true;
    }
  }

  for(const auto &prev : prevState)
  {
    if(newState.find(prev.first) == newState.end())
    {
      modified = true;
      break;
    }
  }

  prevState = std::move(newState);

  return modified;
}
