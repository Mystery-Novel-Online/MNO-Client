#include "theme_mode_reader.h"
#include "file_functions.h"
#include "modules/managers/localization_manager.h"
#include <AOApplication.h>
#include <QDir>

ThemeModeReader::ThemeModeReader(QString filePath)
{

  mTimeModes = {};
  mFilePath = filePath;
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


  if(file_exists(filePath + "/theme.json"))
  {
    m_GamemodeModule = new ThemeModuleReader(filePath, "theme.json");

    ReadFromFile(filePath + "/theme.json");
    SetTargetObject("config");
    QStringList boolConfigs = {"enable_single_shout" , "enable_music_and_area_list_separation", "detatchable_viewport", "enable_single_effect", "enable_single_wtce", "enable_const_music_speed", "enable_showname_image", "enable_highlighting", "enable_button_images", "enable_label_images", "enable_cycle_ding", "use_toggles"};

    for (const QString& boolName : boolConfigs)
    {
      if (isValueExists(boolName))
      {
        mConfigToggles[boolName] = getBoolValue(boolName);
      }
    }

    if(isValueExists("music_scroll_speed"))
    {
      mMusicScrollSpeed = getIntValue("music_scroll_speed");
    }

    if(isValueExists("timer_number"))
    {
      mTimerNumber = getIntValue("timer_number");
    }


    QJsonArray jsonColoursArray = getArrayValue("colors");
    QJsonArray jsonHighlightsArray = getArrayValue("highlights");
    QJsonArray jsonSoundsArray = getArrayValue("sounds");

    for(QJsonValueRef colourObject : jsonColoursArray)
    {
      SetTargetObject(colourObject.toObject());
      mTextColours[getStringValue("color")].code = getStringValue("code");
    }

    for(QJsonValueRef soundObject : jsonSoundsArray)
    {
      SetTargetObject(soundObject.toObject());
      mSounds[getStringValue("sound")] = getStringValue("file");
    }

    for(QJsonValueRef highlights : jsonHighlightsArray)
    {
      SetTargetObject(highlights.toObject());
      mCourtroomFontHighlights[getStringValue("chars")].chars = getStringValue("chars");
      mCourtroomFontHighlights[getStringValue("chars")].color = getStringValue("color");
      mCourtroomFontHighlights[getStringValue("chars")].keepCharacters = getBoolValue("keep_characters");
    }

    SetThemeJsonObject("courtroom");
    mCourtroomWidgetPositions = {};
    mCourtroomWidgetFonts = {};

    for (const QString &key : mTargetObject.keys())
    {
      QJsonObject obj = mTargetObject[key].toObject();

      if(obj.contains("position"))
      {
        pos_size_type return_value;
        return_value.x = obj["position"].toObject()["x"].toInt();
        return_value.y = obj["position"].toObject()["y"].toInt();
        return_value.width = obj["position"].toObject()["width"].toInt();
        return_value.height = obj["position"].toObject()["height"].toInt();
        mCourtroomWidgetPositions[key] = return_value;
      }

      if(obj.contains("settings"))
      {
        if(obj["settings"].toObject().contains("spacing"))
        {
          mWidgetSpacings[key].setX(obj["settings"].toObject()["spacing"].toObject()["x"].toInt());
          mWidgetSpacings[key].setY(obj["settings"].toObject()["spacing"].toObject()["y"].toInt());
        }
      }

      if(obj.contains("font"))
      {
        widgetFontStruct widgetFont;

        if(obj["font"].toObject().contains("name")) widgetFont.font = obj["font"].toObject()["name"].toString();
        if(obj["font"].toObject().contains("color")) widgetFont.color = obj["font"].toObject()["color"].toString();
        if(obj["font"].toObject().contains("align")) widgetFont.align = obj["font"].toObject()["align"].toString();


        widgetFont.size = obj["font"].toObject()["size"].toInt();
        widgetFont.bold = obj["font"].toObject()["bold"].toBool();
        widgetFont.sharp = obj["font"].toObject()["sharp"].toBool();
        widgetFont.outline = obj["font"].toObject()["outline"].toBool();

        mCourtroomWidgetFonts[key] = widgetFont;


      }
    }

    SetThemeJsonObject("lobby");
    mLobbyWidgetPositions = {};
    mLobbyWidgetFonts = {};

    for (const QString &key : mTargetObject.keys())
    {
      QJsonObject obj = mTargetObject[key].toObject();

      if(obj.contains("position"))
      {
        pos_size_type return_value;
        return_value.x = obj["position"].toObject()["x"].toInt();
        return_value.y = obj["position"].toObject()["y"].toInt();
        return_value.width = obj["position"].toObject()["width"].toInt();
        return_value.height = obj["position"].toObject()["height"].toInt();
        mLobbyWidgetPositions[key] = return_value;

      }

      if(obj.contains("font"))
      {
        widgetFontStruct widgetFont;

        if(obj["font"].toObject().contains("name")) widgetFont.font = obj["font"].toObject()["name"].toString();

        if(obj["font"].toObject().contains("color")) widgetFont.color = obj["font"].toObject()["color"].toString();

        if(obj["font"].toObject().contains("align")) widgetFont.align = obj["font"].toObject()["align"].toString();


        widgetFont.size = obj["font"].toObject()["size"].toInt();
        widgetFont.bold = obj["font"].toObject()["bold"].toBool();
        widgetFont.sharp = obj["font"].toObject()["sharp"].toBool();
        widgetFont.outline = obj["font"].toObject()["outline"].toBool();

        mLobbyWidgetFonts[key] = widgetFont;


      }
    }

    QJsonArray tabsArray = mMainObject["tabs"].toArray();
    mTabWidgets = {};
    for(QJsonValueRef tabData : tabsArray)
    {
      SetTargetObject(tabData.toObject());
      mTabWidgets[getStringValue("tab_name").toLower()] = getStringArrayValue("widgets");
    }


    QJsonArray layersArray = mMainObject["layers"].toArray();
    mWidgetLayers = {};
    for(QJsonValueRef widgetLayer : layersArray)
    {
      SetTargetObject(widgetLayer.toObject());
      mWidgetLayers[getStringValue("widget_name").toLower()] = getStringArrayValue("children");
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
  QString gameModesPath = mFilePath + "/times/";
  for (const QString &i_folder : QDir(AOApplication::getInstance()->get_case_sensitive_path(gameModesPath)).entryList(QDir::Dirs))
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    mTimeModes[i_folder] = new ThemeModeReader(mFilePath + "/times/" + i_folder);
    m_TimeModules[i_folder] = new ThemeModuleReader(mFilePath + "/times/" + i_folder, "theme.json");
  }
}

void ThemeModeReader::LoadModules()
{
  m_ModuleNames.clear();
  m_ThemeModules = {};

  QDir modulesDirectory(mFilePath + "/modules");
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

QStringList ThemeModeReader::getThemeDirOrder()
{
  QStringList dirs = {};

  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();
  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    QString l_moduleDir = module->getDirectoryPath();
    dirs.append(l_moduleDir + "/localization/" + LocalizationManager::get().getLocalizationCode());
    dirs.append(l_moduleDir);
  }
  return dirs;
}

void ThemeModeReader::SetTime(QString time)
{
  if(mTimeModes.contains(time))
  {
    mCurrentTimeName = time;
    mCurrentTime = mTimeModes[time];
  }
  else
  {
    mCurrentTimeName = "default";
    mCurrentTime = nullptr;
  }
}

QColor ThemeModeReader::getChatlogColour(QString t_type)
{

  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(COURTROOM);
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
  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(COURTROOM);
  for(ThemeScene * scene : loadOrder)
  {
    if(scene->containsChatlogBold(t_type)) return scene->getChatlogBold(t_type);
  }
  return false;
}

bool ThemeModeReader::getContainsChatlogColour(QString t_type)
{
  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(COURTROOM);
  for(ThemeScene * scene : loadOrder)
  {
    if(scene->containsChatlogColor(t_type)) return true;
  }
  return false;
}

bool ThemeModeReader::getContainsChatlogBool(QString t_type)
{
  QVector<ThemeScene*> loadOrder = getSceneLoadOrder(COURTROOM);
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

QHash<QString, dialogueHighlights> ThemeModeReader::getHighlightColors()
{
  QHash<QString, dialogueHighlights> returnValue = mCourtroomFontHighlights;
  if(mCurrentTime != nullptr)
  {
    QHashIterator<QString, dialogueHighlights> i(mCurrentTime->mCourtroomFontHighlights);
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

QMap<QString, DR::ColorInfo> ThemeModeReader::getTextColors()
{
  QMap<QString, DR::ColorInfo> returnValue = mTextColours;
  if(mCurrentTime != nullptr)
  {
    QMapIterator<QString, DR::ColorInfo> i(mCurrentTime->mTextColours);
    while (i.hasNext())
    {
      i.next();
      returnValue[i.key()].code = i.value().code;
    }
  }
  return returnValue;
}

bool ThemeModeReader::containsSoundName(QString t_soundName)
{
  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();
  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    if(module->getContainsSound(t_soundName)) return true;
  }
  return false;
}

QString ThemeModeReader::getSoundName(QString t_soundName)
{
  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();

  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    if(module->getContainsSound(t_soundName)) return module->getSoundFile(t_soundName);
  }

  return "";
}

bool ThemeModeReader::containsSettingBool(QString t_setting)
{
  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();
  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    if(module->getContainsBool(t_setting)) return true;
  }
  return false;
}

bool ThemeModeReader::getSettingBool(QString t_setting)
{

  QVector<ThemeModuleReader *> modulesList = getModuleLoadOrder();

  for(ThemeModuleReader * module : modulesList)
  {
    if(module == nullptr) continue;
    if(module->getContainsBool(t_setting)) return module->getSettingBool(t_setting);
  }

  return false;
}

QVector2D ThemeModeReader::getWidgetSpacing(QString t_name)
{
  QVector<ThemeScene*> readOrder = getSceneLoadOrder(COURTROOM);

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
  if(mCurrentTime != nullptr)
  {
    if(mCurrentTime->mMusicScrollSpeed != -1) return mCurrentTime->mMusicScrollSpeed;
  }
  if(mMusicScrollSpeed != -1)
  {
    return mMusicScrollSpeed;
  }
  return -1;
}

int ThemeModeReader::getTimerNumber()
{
  if(mCurrentTime != nullptr)
  {
    if(mCurrentTime->mTimerNumber != -1) return mCurrentTime->mTimerNumber;
  }
  if(mTimerNumber != -1)
  {
    return mTimerNumber;
  }
  return -1;
}

bool ThemeModeReader::containsWidgetPosition(ThemeSceneType sceneType, QString name)
{
  if(sceneType == COURTROOM) return containsCourtroomWidgetPosition(name);

  if(sceneType == LOBBY) return containsLobbyWidgetPosition(name);
  return false;
}

pos_size_type ThemeModeReader::getWidgetPosition(ThemeSceneType sceneType, QString name)
{
  return getWidgetDimensions(getSceneLoadOrder(sceneType), name);
}

bool ThemeModeReader::containsCourtroomWidgetPosition(QString widget_name)
{
  if(mCurrentTime != nullptr)
  {
    if(mCurrentTime->mCourtroomWidgetPositions.contains(widget_name)) return true;
  }

  for(QString module : m_ModuleNames)
  {
    if(m_ThemeModules[module].contains("default.json"))
    {
      ThemeScene *scene = m_ThemeModules[module]["default.json"]->getThemeScene(COURTROOM);
      if(scene == nullptr) continue;
      WidgetThemeData* widgetData = scene->getWidgetData(widget_name);
      if(widgetData == nullptr) continue;

      pos_size_type position = widgetData->Transform;
      if(position.height != -1 && position.width != -1)
      {
        return true;
      }

    }
  }

  if(mCourtroomWidgetPositions.contains(widget_name)) return true;


  return false;
}

bool ThemeModeReader::containsLobbyWidgetPosition(QString widget_name)
{
  if(mCurrentTime != nullptr)
  {
    if(mCurrentTime->mLobbyWidgetPositions.contains(widget_name)) return true;
  }

  for(QString module : m_ModuleNames)
  {
    if(m_ThemeModules[module].contains("default.json"))
    {
      ThemeScene *scene = m_ThemeModules[module]["default.json"]->getThemeScene(LOBBY);
      if(scene == nullptr) continue;
      WidgetThemeData* widgetData = scene->getWidgetData(widget_name);
      if(widgetData == nullptr) continue;

      pos_size_type position = widgetData->Transform;
      if(position.height != -1 && position.width != -1)
      {
        return true;
      }

    }
  }

  if(mLobbyWidgetPositions.contains(widget_name)) return true;


  return false;
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
  for (ThemeScene *scene : t_readOrder)
  {
    if(scene == nullptr) continue;
    WidgetThemeData* widgetData = scene->getWidgetData(t_name);
    if(widgetData == nullptr) continue;
    pos_size_type position = widgetData->Transform;
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

  if(m_TimeModules.contains(mCurrentTimeName))
  {
    if(m_TimeModules[mCurrentTimeName] != nullptr) returnValue.append(m_TimeModules[mCurrentTimeName]);
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
