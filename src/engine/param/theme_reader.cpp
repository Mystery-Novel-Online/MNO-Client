#include "theme_reader.h"

#include <modules/theme/thememanager.h>
#include "engine/fs/fs_reading.h"


void ThemeReader::LoadTheme(QString themeName)
{
  m_ThemeName = themeName;
  m_ThemeDirectory = FS::Paths::FindDirectory("themes/" + m_ThemeName);
  m_GameModeCollection = {};
  m_GameModeCollection["default"] = new ThemeModeReader(m_ThemeDirectory);

  QString gameModesPath = m_ThemeDirectory + "/gamemodes/";
  for (const QString &i_folder : QDir(AOApplication::getInstance()->get_case_sensitive_path(gameModesPath)).entryList(QDir::Dirs))
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    m_GameModeCollection[i_folder] = new ThemeModeReader(m_ThemeDirectory + "/gamemodes/" + i_folder);
  }

}

void ThemeReader::SetGamemode(QString gamemode)
{
  if(m_GameModeCollection.contains(gamemode))
  {
    m_GameModeCurrentName = gamemode;
    m_GameModeCurrent = m_GameModeCollection[gamemode];
  }
  else
  {
    m_GameModeCurrentName = "default";
    m_GameModeCurrent = m_GameModeCollection["default"];
  }
}

void ThemeReader::SetTimeOfDay(QString time)
{
  m_TimeOfDay = time;
  if(m_GameModeCurrent != nullptr) m_GameModeCurrent->SetTimeOfDay(time);
}

QColor ThemeReader::getChatlogColor(QString type)
{
  QColor return_value = QColor(0,0,0);

  if(m_GameModeCurrent != nullptr)
  {
    if(m_GameModeCurrent->getContainsChatlogColour(type)) return m_GameModeCurrent->getChatlogColour(type);
  }

  if(m_GameModeCollection["default"]->getContainsChatlogColour(type))
  {
    return m_GameModeCollection["default"]->getChatlogColour(type);
  }

  return return_value;
}

bool ThemeReader::getChatlogBool(QString t_type)
{
  bool return_value = false;

  if(m_GameModeCurrent != nullptr)
  {
    if(m_GameModeCurrent->getContainsChatlogBool(t_type)) return m_GameModeCurrent->getChatlogBool(t_type);
  }

  if(m_GameModeCollection["default"]->getContainsChatlogBool(t_type))
  {
    return m_GameModeCollection["default"]->getChatlogBool(t_type);
  }

  return return_value;
}

QStringList ThemeReader::getThemeDirOrder()
{
  QStringList l_DirectoryOrder = {};

  for(ThemeModeReader* r_ThemeModeReader : GetGamemodeOrder())
  {
    l_DirectoryOrder.append(r_ThemeModeReader->GetDirectoryLoadOrder());
  }

  return l_DirectoryOrder;
}

QVector2D ThemeReader::GetWidgetSpacing(QString widget_name)
{
  QVector2D return_value(-1, -1);
  if(m_GameModeCurrent != nullptr)
  {
    return_value = m_GameModeCurrent->getWidgetSpacing(widget_name);
  }

  if(return_value.x() == -1 || return_value.y() == -1)
  {
    return_value = m_GameModeCollection["default"]->getWidgetSpacing(widget_name);
  }

  double resize = 1.0;
  return_value.setX((int)((double)return_value.x() * resize));
  return_value.setY((int)((double)return_value.y() * resize));

  return return_value;

}

int ThemeReader::getMusicScrollSpeed()
{
  if(m_GameModeCurrent != nullptr)
  {
    if(m_GameModeCurrent->getMusicScrollSpeed() != -1) return m_GameModeCurrent->getMusicScrollSpeed();
  }
  if(m_GameModeCollection["default"]->getMusicScrollSpeed() != -1)
  {
    return m_GameModeCollection["default"]->getMusicScrollSpeed();
  }

  return -1;
}

int ThemeReader::getTimerNumber()
{
  if(m_GameModeCurrent != nullptr)
  {
    if(m_GameModeCurrent->getTimerNumber() != -1) return m_GameModeCurrent->getTimerNumber();
  }
  if(m_GameModeCollection["default"]->getTimerNumber() != -1)
  {
    return m_GameModeCollection["default"]->getTimerNumber();
  }

  return 0;
}

QVector<QStringList> ThemeReader::GetLayers()
{
  if(m_GameModeCurrent != nullptr)
  {
    QVector<QStringList> layers = m_GameModeCurrent->getLayers();

    if(layers.count() != 0) return layers;
  }

  QVector<QStringList> layers = m_GameModeCollection["default"]->getLayers();
  if(layers.count() != 0) return layers;
  return {};
}

  QStringList ThemeReader::GetLayerChildren(QString t_widget)
{
  QStringList l_returnData = {};
  if(m_GameModeCurrent != nullptr)
  {
    QVector<QStringList> l_rawLayers = m_GameModeCurrent->getLayers();

    if(l_rawLayers.count() != 0)
    {
      for(QStringList l_layers : l_rawLayers)
      {
        if(l_layers[0] == t_widget)
        {
          l_returnData = l_layers;
          l_returnData.removeAt(0);
          return l_returnData;
        }
      }
    }
  }

  QVector<QStringList> l_rawLayers = m_GameModeCollection["default"]->getLayers();

  if(l_rawLayers.count() != 0)
  {
    for(QStringList l_layers : l_rawLayers)
    {
      if(l_layers[0] == t_widget)
      {
        l_returnData = l_layers;
        l_returnData.removeAt(0);
        return l_returnData;
      }
    }
  }

  return {};
}

QVector<ThemeTabInfo> ThemeReader::getTabs()
{
  QVector<ThemeTabInfo> return_data = {};

  if(m_GameModeCurrent != nullptr && m_GameModeCurrent != m_GameModeCollection["default"])
  {
    return_data = m_GameModeCurrent->getTabs();
    if(return_data.length() > 0) return return_data;
  }
  return_data = m_GameModeCollection["default"]->getTabs();
  return return_data;
}

bool ThemeReader::GetConfigBool(QString boolValue)
{
  QVector<ThemeModuleReader *> l_modules = RetrieveModuleOrder();

  for(ThemeModuleReader * r_module : l_modules)
  {
    if(r_module != nullptr)
    {
      if(r_module->getContainsBool(boolValue)) return r_module->getSettingBool(boolValue);
    }
  }

  return false;
}

QString ThemeReader::GetConfigSoundName(QString soundName)
{
  QVector<ThemeModuleReader *> l_modules = RetrieveModuleOrder();

  for(ThemeModuleReader * r_module : l_modules)
  {
    if(r_module != nullptr)
    {
      if(r_module->getContainsSound(soundName)) return r_module->getSoundFile(soundName);
    }
  }

  return "";
}

QVector<QStringList> ThemeReader::GetColorsHighlights()
{
  QVector<QStringList> f_vec;
  QHash<QString, ThemeHighlight> returnValue = m_GameModeCollection["default"]->GetFontColorsHighlights(); ;
  if(m_GameModeCurrent != nullptr && m_GameModeCurrent != m_GameModeCollection["default"])
  {
    QHashIterator<QString, ThemeHighlight> i(m_GameModeCurrent->GetFontColorsHighlights());
    while (i.hasNext())
    {
      i.next();
      returnValue[i.key()].chars = i.value().chars;
      returnValue[i.key()].color = i.value().color;
      returnValue[i.key()].keepCharacters = i.value().keepCharacters;
    }
  }
  QHashIterator<QString, ThemeHighlight> ThemeHighlight(returnValue);
  while(ThemeHighlight.hasNext())
  {
    ThemeHighlight.next();
    f_vec.append({QString::fromStdString(ThemeHighlight.value().chars), QString::fromStdString(ThemeHighlight.value().color), ThemeHighlight.value().keepCharacters ? "1" : "0"});
  }

  return f_vec;

}

QMap<QString, DR::ColorInfo> ThemeReader::GetColorsDefault()
{
  QMap<QString, DR::ColorInfo> returnValue = m_GameModeCollection["default"]->GetFontColorsDefault();
  if(m_GameModeCurrent != nullptr && m_GameModeCurrent != m_GameModeCollection["default"])
  {
    QMapIterator<QString, DR::ColorInfo> i(m_GameModeCurrent->GetFontColorsDefault());
    while (i.hasNext())
    {
      i.next();
      returnValue[i.key()].code = i.value().code;
    }
  }
  return returnValue;
}

bool ThemeReader::IsPixmapExist(QString path)
{
  if(m_GameModeCurrent->mThemeImages.contains(path))
  {
    return true;
  }
  else if(m_GameModeCollection.contains("default"))
  {
    if(m_GameModeCollection["default"]->mThemeImages.contains(path))
    {
      return true;
    }
  }
  return false;
}

AOPixmap ThemeReader::GetCachedPixmap(QString path)
{
  if(m_GameModeCurrent->mThemeImages.contains(path))
  {
    return m_GameModeCurrent->mThemeImages[path];
  }
  else if(m_GameModeCollection.contains("default"))
  {
    if(m_GameModeCollection["default"]->mThemeImages.contains(path))
    {
      return m_GameModeCollection["default"]->mThemeImages[path];
    }
  }
  return AOPixmap();
}

widgetFontStruct ThemeReader::GetFontData(ThemeSceneType sceneType, QString element)
{
  widgetFontStruct return_value;
  double resize = 1.0;

  if(m_GameModeCurrent != nullptr)
  {
    if(m_GameModeCurrent->containsWidgetFont(sceneType, element))
    {
      return_value = m_GameModeCurrent->getWidgetFont(sceneType, element);
      return_value.size = (int)((double)return_value.size * resize);
      return return_value;
    }
  }

  if(m_GameModeCollection["default"]->containsWidgetFont(sceneType, element))
  {
    return_value = m_GameModeCollection["default"]->getWidgetFont(sceneType, element);
  }
  return_value.size = (int)((double)return_value.size * resize);

  return return_value;
}

RPRect ThemeReader::GetWidgetTransform(ThemeSceneType sceneType, QString element)
{
  RPRect return_value;

  double resize = ThemeManager::get().getResize();

  if(m_GameModeCurrent != nullptr)
  {
    if(m_GameModeCurrent->containsWidgetPosition(sceneType, element))
    {
      return_value = m_GameModeCurrent->getWidgetPosition(sceneType, element);

      return_value.x = (int)((double)return_value.x * resize);
      return_value.y = (int)((double)return_value.y * resize);
      return_value.width = (int)((double)return_value.width * resize);
      return_value.height = (int)((double)return_value.height * resize);

      return return_value;
    }
  }

  if(m_GameModeCollection["default"]->containsWidgetPosition(sceneType, element))
  {
    return_value = m_GameModeCollection["default"]->getWidgetPosition(sceneType, element);
  }

  return_value.x = (int)((double)return_value.x * resize);
  return_value.y = (int)((double)return_value.y * resize);
  return_value.width = (int)((double)return_value.width * resize);
  return_value.height = (int)((double)return_value.height * resize);

  return return_value;
}

widgetFontStruct ThemeReader::GetFontDataPairing(QString element, QString position)
{
  widgetFontStruct return_value;

  double resize = 1.0;
  QString name_alignment = element + "_" + position;

  if(m_GameModeCurrent != nullptr)
  {
    if(m_GameModeCurrent->containsWidgetFont(ThemeSceneType::SceneType_Courtroom, name_alignment))
    {
      return_value = m_GameModeCurrent->getWidgetFont(ThemeSceneType::SceneType_Courtroom, name_alignment);
      return_value.size = (int)((double)return_value.size * resize);
      return return_value;
    }
    else if(m_GameModeCurrent->containsWidgetFont(ThemeSceneType::SceneType_Courtroom, element))
    {
      return_value = m_GameModeCurrent->getWidgetFont(ThemeSceneType::SceneType_Courtroom, element);
      return_value.size = (int)((double)return_value.size * resize);
      return return_value;
    }
  }


  if(m_GameModeCollection.contains("default"))
  {
    if(m_GameModeCollection["default"]->containsWidgetFont(ThemeSceneType::SceneType_Courtroom, name_alignment)) return_value = m_GameModeCollection["default"]->getWidgetFont(ThemeSceneType::SceneType_Courtroom, name_alignment);
    else if(m_GameModeCollection["default"]->containsWidgetFont(ThemeSceneType::SceneType_Courtroom, element))
    {
      return_value = m_GameModeCollection["default"]->getWidgetFont(ThemeSceneType::SceneType_Courtroom, element);
    }
  }

  return_value.size = (int)((double)return_value.size * resize);
  return return_value;

  return return_value;
}


