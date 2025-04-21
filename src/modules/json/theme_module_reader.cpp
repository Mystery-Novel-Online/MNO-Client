#include "theme_module_reader.h"
#include "file_functions.h"

ThemeModuleReader::ThemeModuleReader(QString t_moduleDirectory, QString t_moduleName)
{
  m_moduleDirectory = t_moduleDirectory;
  QString modulePath = m_moduleDirectory + "/" + t_moduleName;

  if(file_exists(modulePath))
  {
    ReadFromFile(modulePath);
    ParseModule();
  }
}

void ThemeModuleReader::ParseModule()
{
  m_WidgetLayers = {};
  m_themeTabs = {};

  ParseModuleConfig();
  m_CourtroomScene = ParseScene("courtroom");
  m_LobbyScene = ParseScene("lobby");
  ParseLayers();
  ParseTabs();
}

void ThemeModuleReader::ParseModuleConfig()
{
  SetTargetObject("config");
  QStringList configOptionNames = {"enable_single_shout" , "enable_music_and_area_list_separation", "enable_single_effect",  "detatchable_viewport", "enable_single_wtce", "enable_const_music_speed", "enable_showname_image", "enable_highlighting", "enable_button_images", "enable_label_images", "enable_cycle_ding", "use_toggles"};

  for (const QString& optionName : configOptionNames) {
    if (isValueExists(optionName)) m_configBooleans[optionName] = getBoolValue(optionName);
  }
  
  if(isValueExists("music_scroll_speed")) m_configMusicSpeed = getIntValue("music_scroll_speed");
  if(isValueExists("timer_number")) m_configTimer = getIntValue("timer_number");

  QJsonArray configColorsArray = getArrayValue("colors");
  QJsonArray configHighlightsArray = getArrayValue("highlights");
  QJsonArray configSoundsArray = getArrayValue("sounds");

  for(QJsonValueRef colorValue : configColorsArray)
  {
    SetTargetObject(colorValue.toObject());
    m_configColors[getStringValue("color")].code = getStringValue("code");
  }

  for(QJsonValueRef soundValue : configSoundsArray)
  {
    SetTargetObject(soundValue.toObject());
    m_configSounds[getStringValue("sound")] = getStringValue("file");
  }

  for(QJsonValueRef highlighValue : configHighlightsArray)
  {
    SetTargetObject(highlighValue.toObject());
    m_configHighlights[getStringValue("chars")].chars = getStringValue("chars");
    m_configHighlights[getStringValue("chars")].color = getStringValue("color");
    m_configHighlights[getStringValue("chars")].keepCharacters = getBoolValue("keep_characters");
  }
}

void ThemeModuleReader::ParseLayers()
{

  SetTargetObject(mMainObject);
  QJsonArray layersObjectArray = getArrayValue("layers");

  for(QJsonValueRef i : layersObjectArray)
  {
    QStringList layerInfo = {};
    SetTargetObject(i.toObject());
    QString widget_name = getStringValue("widget_name");

    if(!widget_name.isEmpty())
    {
      layerInfo.append(widget_name);

      if(widget_name == "viewport")
      {
        layerInfo.append("viewport_transition");
      }

      QStringList childArray = getStringArrayValue("children");

      for(QString layerChild : childArray)
      {
        if(!layerChild.isEmpty()) layerInfo.append(layerChild);
      }

      if(widget_name == "viewport")
      {
        layerInfo.append("viewport_overlay");
      }
      m_WidgetLayers.append(layerInfo);
    }
  }

}

void ThemeModuleReader::ParseTabs()
{
  SetTargetObject(mMainObject);
  QJsonArray tabsObjectArray = getArrayValue("tabs");

  for(QJsonValueRef tabObject : tabsObjectArray)
  {
    SetTargetObject(tabObject.toObject());
    ThemeTabInfo themeTab;
    themeTab.m_Name = getStringValue("tab_name").toLower();
    themeTab.m_WidgetContents = getStringArrayValue("widgets");

    if(tabObject.toObject().contains("group"))
    {
      themeTab.m_Group = getStringValue("group");
    }

    if(tabObject.toObject().contains("position"))
    {
      SetTargetObject("position");
      themeTab.m_transform.x = getIntValue("x");
      themeTab.m_transform.y = getIntValue("y");
    }

    m_themeTabs.append(themeTab);
  }

}

ThemeScene *ThemeModuleReader::ParseScene(QString t_scene)
{
  if(!mMainObject.contains(t_scene)) return nullptr;

  SwitchModuleTarget(t_scene);


  ThemeScene *newScene = new ThemeScene();

  for (const QString &key : mTargetObject.keys())
  {
    QJsonObject obj = mTargetObject[key].toObject();

    if(obj.contains("position"))
    {
      pos_size_type widgetTransform;
      widgetTransform.x = obj["position"].toObject()["x"].toInt(); widgetTransform.y = obj["position"].toObject()["y"].toInt();
      widgetTransform.width = obj["position"].toObject()["width"].toInt(); widgetTransform.height = obj["position"].toObject()["height"].toInt();
      newScene->setWidgetTransform(key, widgetTransform);
    }

    if(obj.contains("settings"))
    {
      if(obj["settings"].toObject().contains("spacing"))
      {
        QVector2D widgetSpacing;
        widgetSpacing.setX(obj["settings"].toObject()["spacing"].toObject()["x"].toInt());
        widgetSpacing.setY(obj["settings"].toObject()["spacing"].toObject()["y"].toInt());
        newScene->setWidgetSpacing(key, widgetSpacing);
      }
    }

    if(key == "sfx_list")
    {
      if(obj["settings"].toObject().contains("found_sfx_color")) m_configSounds["found_sfx_color"] = obj["settings"].toObject()["found_sfx_color"].toString();
      if(obj["settings"].toObject().contains("missing_sfx_color")) m_configSounds["missing_sfx_color"] = obj["settings"].toObject()["missing_sfx_color"].toString();
    }

    if(key == "area_list")
    {
      if(obj["settings"].toObject().contains("area_free_color")) m_configSounds["area_free_color"] = obj["settings"].toObject()["area_free_color"].toString();
    }

    if(key == "music_list")
    {
      if(obj["settings"].toObject().contains("found_song_color")) m_configSounds["found_song_color"] = obj["settings"].toObject()["found_song_color"].toString();
      if(obj["settings"].toObject().contains("missing_song_color")) m_configSounds["missing_song_color"] = obj["settings"].toObject()["missing_song_color"].toString();
    }




    if(obj.contains("font"))
    {
      widgetFontStruct *widgetFont = new widgetFontStruct();
      QJsonObject widgetFontObject = obj["font"].toObject();

      if(widgetFontObject.contains("name")) widgetFont->font = obj["font"].toObject()["name"].toString();
      if(widgetFontObject.contains("color")) widgetFont->color = obj["font"].toObject()["color"].toString();
      if(widgetFontObject.contains("align")) widgetFont->align = obj["font"].toObject()["align"].toString();


      widgetFont->size = widgetFontObject["size"].toInt();
      widgetFont->bold = widgetFontObject["bold"].toBool();
      widgetFont->sharp = widgetFontObject["sharp"].toBool();
      widgetFont->outline = widgetFontObject["outline"].toBool();

      newScene->setWidgetFont(key, widgetFont);

      if(key == "ic_chatlog")
      {
        ParseSubFont(widgetFontObject, "message", newScene);
        ParseSubFont(widgetFontObject, "system", newScene);
        ParseSubFont(widgetFontObject, "showname", newScene);
        ParseSubFont(widgetFontObject, "selfname", newScene);
      }
    }
  }

  return newScene;
}

void ThemeModuleReader::ParseSubFont(QJsonObject t_fontObject, QString t_fontType, ThemeScene *t_scene)
{
  if(t_fontObject.contains(t_fontType + "_color"))
  {
    t_scene->setChatlogColor(t_fontType, t_fontObject[t_fontType + "_color"].toString());
  }

  if(t_fontObject.contains(t_fontType + "_bold"))
  {
    t_scene->setChatlogBold(t_fontType, t_fontObject[t_fontType + "_bold"].toBool());
  }
}

void ThemeModuleReader::SwitchModuleTarget(QString t_target)
{
  SetTargetObject(mMainObject);
  SetTargetObject(t_target);
}

ThemeScene *ThemeModuleReader::getThemeScene(ThemeSceneType t_scene)
{
  switch (t_scene)
  {
    case LOBBY:
      return m_LobbyScene;

    case COURTROOM:
    return m_CourtroomScene;

    default:
      return nullptr;
    }
}

bool ThemeModuleReader::getContainsLayers()
{
  return m_WidgetLayers.count() != 0;
}

QVector<QStringList> ThemeModuleReader::getWidgetLayers()
{
  return m_WidgetLayers;
}

QVector<ThemeTabInfo> ThemeModuleReader::getTabs()
{
  return m_themeTabs;
}

bool ThemeModuleReader::getContainsBool(QString t_setting)
{
  return m_configBooleans.contains(t_setting);
}

bool ThemeModuleReader::getSettingBool(QString t_setting)
{
  return m_configBooleans[t_setting];
}

bool ThemeModuleReader::getContainsSound(QString t_sfxName)
{
  return m_configSounds.contains(t_sfxName);
}

QString ThemeModuleReader::getSoundFile(QString t_sfxName)
{
  return m_configSounds[t_sfxName];
}

QString ThemeModuleReader::getDirectoryPath()
{
  return m_moduleDirectory;
}
