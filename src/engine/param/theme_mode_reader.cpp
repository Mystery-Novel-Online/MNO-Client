#include "theme_mode_reader.h"
#include "engine/fs/fs_reading.h"
#include "engine/system/localization.h"

ThemeModeReader::ThemeModeReader(QString filePath)
{

  m_TimeOfDayReaders = {};
  m_FilePath = filePath;
  //Load Images
  QDir directory(filePath);

  QStringList filters = {"*.png", "*.webp", "*.gif"};
  directory.setNameFilters(filters);

  QStringList fileList = directory.entryList();

  foreach (const QString &fileName, fileList)
  {
    QString filePath = directory.absoluteFilePath(fileName);
    mThemeImages[filePath] = AOPixmap(filePath);
  }


  if(FS::Checks::FileExists(filePath + "/theme.json"))
  {
    m_GamemodeModule = new ThemeModuleReader(filePath, "theme.json");

    ReadFromFile(filePath + "/theme.json");
    SetTargetObject("config");

    if(isValueExists("music_scroll_speed"))
    {
      m_MusicDisplayScrollSpeed = getIntValue("music_scroll_speed");
    }

    if(isValueExists("timer_number"))
    {
      m_TimerCount = getIntValue("timer_number");
    }


    QJsonArray jsonColoursArray = getArrayValue("colors");
    QJsonArray jsonHighlightsArray = getArrayValue("highlights");
    QJsonArray jsonSoundsArray = getArrayValue("sounds");

    for(QJsonValueRef colourObject : jsonColoursArray)
    {
      SetTargetObject(colourObject.toObject());
      m_CourtroomFontColorsDefault[getStringValue("color")].code = getStringValue("code");
    }

    for(QJsonValueRef highlights : jsonHighlightsArray)
    {
      SetTargetObject(highlights.toObject());
      m_CourtroomFontColorsHighlights[getStringValue("chars")].chars = getStringValue("chars");
      m_CourtroomFontColorsHighlights[getStringValue("chars")].color = getStringValue("color");
      m_CourtroomFontColorsHighlights[getStringValue("chars")].keepCharacters = getBoolValue("keep_characters");
    }

  }
  LoadTimeMode();
  LoadModules();
}

ThemeModeReader::~ThemeModeReader()
{
  delete m_GamemodeModule;
}

void ThemeModeReader::SetThemeJsonObject(QString category)
{
  SetTargetObject(mMainObject);
  SetTargetObject(category);
}

void ThemeModeReader::LoadTimeMode()
{
  m_TimeModules = {};
  QString gameModesPath = m_FilePath + "/times/";
  for (const QString &i_folder : QDir(AOApplication::getInstance()->get_case_sensitive_path(gameModesPath)).entryList(QDir::Dirs))
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    m_TimeOfDayReaders[i_folder] = new ThemeModeReader(m_FilePath + "/times/" + i_folder);
    m_TimeModules[i_folder] = new ThemeModuleReader(m_FilePath + "/times/" + i_folder, "theme.json");
  }
}

void ThemeModeReader::LoadModules()
{
  m_ModuleNames.clear();
  m_ThemeModules = {};

  QDir modulesDirectory(m_FilePath + "/modules");
  QStringList moduleSubDirectories = modulesDirectory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

  for (const QString &moduleDirectory : moduleSubDirectories)
  {
    m_ModuleNames.append(moduleDirectory);
    qDebug() << "[Theme][Modules] Found module directory:" + moduleDirectory;
    QDir fullModulePath(modulesDirectory.filePath(moduleDirectory));

    QStringList moduleFiles = fullModulePath.entryList(QStringList() << "*.json", QDir::Files);
    for (const QString &module : moduleFiles)
    {
      if(!m_ThemeModules[moduleDirectory].contains(module))
      {
        m_ThemeModules[moduleDirectory][module] = new ThemeModuleReader(modulesDirectory.filePath(moduleDirectory), module);
      }
    }
  }

}

QStringList ThemeModeReader::GetDirectoryLoadOrder()
{
  QStringList dirs = {};

  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();
  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    QString l_moduleDir = module->getDirectoryPath();
    dirs.append(l_moduleDir + "/localization/" + engine::system::localization::getCode());
    dirs.append(l_moduleDir);
  }
  return dirs;
}

void ThemeModeReader::SetTimeOfDay(QString time)
{
  if(m_TimeOfDayReaders.contains(time))
  {
    m_TimeOfDayCurrentName = time;
    m_TimeOfDayCurrent = m_TimeOfDayReaders[time];
  }
  else
  {
    m_TimeOfDayCurrentName = "default";
    m_TimeOfDayCurrent = nullptr;
  }
}

QColor ThemeModeReader::getChatlogColour(QString t_type)
{

  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(SceneType_Courtroom);
  for(ThemeScene * scene : loadOrder)
  {
    if(scene->containsChatlogColor(t_type))
    {
      return QColor(scene->getChatlogColor(t_type));
    }
  }

  return QColor(0,0,0);
}

bool ThemeModeReader::getChatlogBool(QString t_type)
{
  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(SceneType_Courtroom);
  for(ThemeScene * scene : loadOrder)
  {
    if(scene->containsChatlogBold(t_type)) return scene->getChatlogBold(t_type);
  }
  return false;
}

bool ThemeModeReader::getContainsChatlogColour(QString t_type)
{
  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(SceneType_Courtroom);
  for(ThemeScene * scene : loadOrder)
  {
    if(scene->containsChatlogColor(t_type)) return true;
  }
  return false;
}

bool ThemeModeReader::getContainsChatlogBool(QString t_type)
{
  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(SceneType_Courtroom);
  for(ThemeScene * scene : loadOrder)
  {
    if(scene->containsChatlogBold(t_type)) return true;
  }
  return false;
}

QVector<ThemeTabInfo> ThemeModeReader::getTabs()
{

  QVector<ThemeTabInfo> return_data = {};

  QVector<ThemeModuleReader*> loadOrder = getModuleLoadOrder();

  for(ThemeModuleReader * module : loadOrder)
  {
    return_data = module->getTabs();
    if(return_data.length() > 0) return return_data;
  }

  return return_data;
}

QVector<QStringList> ThemeModeReader::getLayers()
{
  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();
  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    if(module->getContainsLayers()) return module->getWidgetLayers();
  }
  return {};
}

QHash<QString, dialogueHighlights> ThemeModeReader::GetFontColorsHighlights()
{
  QHash<QString, dialogueHighlights> returnValue = m_CourtroomFontColorsHighlights;
  if(m_TimeOfDayCurrent != nullptr)
  {
    QHashIterator<QString, dialogueHighlights> i(m_TimeOfDayCurrent->m_CourtroomFontColorsHighlights);
    while (i.hasNext())
    {
      i.next();
      returnValue[i.key()].chars = i.value().chars;
      returnValue[i.key()].color = i.value().color;
      returnValue[i.key()].keepCharacters = i.value().keepCharacters;
    }
  }
  return returnValue;

}

QMap<QString, DR::ColorInfo> ThemeModeReader::GetFontColorsDefault()
{
  QMap<QString, DR::ColorInfo> returnValue = m_CourtroomFontColorsDefault;
  if(m_TimeOfDayCurrent != nullptr)
  {
    QMapIterator<QString, DR::ColorInfo> i(m_TimeOfDayCurrent->m_CourtroomFontColorsDefault);
    while (i.hasNext())
    {
      i.next();
      returnValue[i.key()].code = i.value().code;
    }
  }
  return returnValue;
}

QVector2D ThemeModeReader::getWidgetSpacing(QString t_name)
{
  QVector<ThemeScene*> readOrder = getSceneLoadOrder(SceneType_Courtroom);

  for (ThemeScene *scene : readOrder)
  {
    if(scene == nullptr) continue;
    WidgetThemeData* widgetData = scene->getWidgetData(t_name);
    if(widgetData == nullptr) continue;
    QVector2D spacing = widgetData->Spacing;
    if(spacing.x() != -1 && spacing.y() != -1) return spacing;
  }

  return QVector2D(-1, -1);
}

int ThemeModeReader::getMusicScrollSpeed()
{
  if(m_TimeOfDayCurrent != nullptr)
  {
    if(m_TimeOfDayCurrent->m_MusicDisplayScrollSpeed != -1) return m_TimeOfDayCurrent->m_MusicDisplayScrollSpeed;
  }
  if(m_MusicDisplayScrollSpeed != -1)
  {
    return m_MusicDisplayScrollSpeed;
  }
  return -1;
}

int ThemeModeReader::getTimerNumber()
{
  if(m_TimeOfDayCurrent != nullptr)
  {
    if(m_TimeOfDayCurrent->m_TimerCount != -1) return m_TimeOfDayCurrent->m_TimerCount;
  }
  if(m_TimerCount != -1)
  {
    return m_TimerCount;
  }
  return -1;
}

bool ThemeModeReader::containsWidgetPosition(ThemeSceneType sceneType, QString name)
{
  if(m_TimeOfDayCurrent != nullptr)
  {
    if(m_TimeOfDayCurrent->m_GamemodeModule != nullptr)
    {
      bool l_timeHasWidget = m_TimeOfDayCurrent->m_GamemodeModule->getContainsSceneWidget(sceneType, name);
      if(l_timeHasWidget) return true;
    }
  }

  for(QString module : m_ModuleNames)
  {
    if(m_ThemeModules[module].contains("default.json"))
    {
      bool l_modHasWidget = m_ThemeModules[module]["default.json"]->getContainsSceneWidget(sceneType, name);
      if(l_modHasWidget) return true;
    }
  }

  if(m_GamemodeModule != nullptr) return m_GamemodeModule->getContainsSceneWidget(sceneType, name);

  return false;
}

pos_size_type ThemeModeReader::getWidgetPosition(ThemeSceneType sceneType, QString name)
{
  return getWidgetDimensions(getSceneLoadOrder(sceneType), name);
}

bool ThemeModeReader::containsWidgetFont(ThemeSceneType t_sceneType, QString widget_name)
{
  QVector<ThemeScene*> readOrder = getSceneLoadOrder(t_sceneType);

  for (ThemeScene *scene : readOrder)
  {
    if(scene == nullptr) continue;
    widgetFontStruct* widgetData = scene->getWidgetFont(widget_name);
    if(widgetData == nullptr) continue;
    return true;
  }

  return false;
}


widgetFontStruct ThemeModeReader::getWidgetFont(ThemeSceneType t_sceneType, QString t_name)
{
  QVector<ThemeScene*> readOrder = getSceneLoadOrder(t_sceneType);

  for (ThemeScene *scene : readOrder)
  {
    if(scene == nullptr) continue;
    widgetFontStruct* widgetData = scene->getWidgetFont(t_name);
    if(widgetData == nullptr) continue;
    return *widgetData;
  }

  return widgetFontStruct();
}

pos_size_type ThemeModeReader::getWidgetDimensions(QVector<ThemeScene *> t_readOrder, QString t_name)
{
  for (ThemeScene *r_SceneToRead : t_readOrder)
  {
    if(r_SceneToRead == nullptr) continue;

    WidgetThemeData* l_WidgetThemeData = r_SceneToRead->getWidgetData(t_name);
    if(l_WidgetThemeData == nullptr) continue;

    pos_size_type position = l_WidgetThemeData->Transform;
    if(position.height != -1 && position.width != -1) return position;
  }
  return pos_size_type();
}


QVector<ThemeScene*> ThemeModeReader::getSceneLoadOrder(ThemeSceneType t_type)
{
  QVector<ThemeScene*> returnValue = {};

  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();

  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    ThemeScene *scene = module->getThemeScene(t_type);
    if(scene != nullptr) returnValue.append(scene);
  }

  return returnValue;
}

QVector<ThemeModuleReader *> ThemeModeReader::getModuleLoadOrder()
{
  QVector<ThemeModuleReader*> returnValue = {};

  if(m_TimeModules.contains(m_TimeOfDayCurrentName))
  {
    if(m_TimeModules[m_TimeOfDayCurrentName] != nullptr) returnValue.append(m_TimeModules[m_TimeOfDayCurrentName]);
  }

  for(QString module : m_ModuleNames)
  {
    if(m_ThemeModules[module].contains("default.json"))
    {
      if(m_ThemeModules[module]["default.json"] != nullptr) returnValue.append(m_ThemeModules[module]["default.json"]);
    }
  }

  if(m_GamemodeModule != nullptr)
  {
    returnValue.append(m_GamemodeModule);
  }

  return returnValue;
}


