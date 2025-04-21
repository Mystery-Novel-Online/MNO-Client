#include "theme_reader.h"
#include "aoapplication.h"
#include <QDir>
#include <modules/theme/thememanager.h>

ThemeReader::ThemeReader()
{

}

void ThemeReader::LoadTheme(QString themeName)
{
  mThemeName = themeName;
  mThemeDirectory = AOApplication::getInstance()->get_package_or_base_path("themes/" + mThemeName);
  mGameModes = {};
  mGameModes["default"] = new ThemeModeReader(mThemeDirectory);

  QString gameModesPath = mThemeDirectory + "/gamemodes/";
  for (const QString &i_folder : QDir(AOApplication::getInstance()->get_case_sensitive_path(gameModesPath)).entryList(QDir::Dirs))
  {
    if (i_folder == "." || i_folder == "..")
      continue;
    mGameModes[i_folder] = new ThemeModeReader(mThemeDirectory + "/gamemodes/" + i_folder);
  }

}

void ThemeReader::SetGamemode(QString gamemode)
{
  if(mGameModes.contains(gamemode))
  {
    mCurrentGamemodeName = gamemode;
    mCurrentGamemode = mGameModes[gamemode];
  }
  else
  {
    mCurrentGamemodeName = "default";
    mCurrentGamemode = mGameModes["default"];
  }
}

void ThemeReader::SetTime(QString time)
{
  mTime = time;
  if(mCurrentGamemode != nullptr) mCurrentGamemode->SetTime(time);
}

QColor ThemeReader::getChatlogColor(QString type)
{
  QColor return_value = QColor(0,0,0);

  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->getContainsChatlogColour(type)) return mCurrentGamemode->getChatlogColour(type);
  }

  if(mGameModes["default"]->getContainsChatlogColour(type))
  {
    return mGameModes["default"]->getChatlogColour(type);
  }

  return return_value;
}

bool ThemeReader::getChatlogBool(QString t_type)
{
  bool return_value = false;

  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->getContainsChatlogBool(t_type)) return mCurrentGamemode->getChatlogBool(t_type);
  }

  if(mGameModes["default"]->getContainsChatlogBool(t_type))
  {
    return mGameModes["default"]->getChatlogBool(t_type);
  }

  return return_value;
}

QStringList ThemeReader::getThemeDirOrder()
{
  QStringList reteurnDirs = {};

  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemodeName != "default") reteurnDirs.append(mCurrentGamemode->getThemeDirOrder());
  }
  reteurnDirs.append(mGameModes["default"]->getThemeDirOrder());
  return reteurnDirs;
}

QVector2D ThemeReader::getWidgetSpacing(QString widget_name)
{
  QVector2D return_value(-1, -1);
  if(mCurrentGamemode != nullptr)
  {
    return_value = mCurrentGamemode->getWidgetSpacing(widget_name);
  }

  if(return_value.x() == -1 || return_value.y() == -1)
  {
    return_value = mGameModes["default"]->getWidgetSpacing(widget_name);
  }

  double resize = ThemeManager::get().getResize();
  return_value.setX((int)((double)return_value.x() * resize));
  return_value.setY((int)((double)return_value.y() * resize));

  return return_value;

}

int ThemeReader::getMusicScrollSpeed()
{
  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->getMusicScrollSpeed() != -1) return mCurrentGamemode->getMusicScrollSpeed();
  }
  if(mGameModes["default"]->getMusicScrollSpeed() != -1)
  {
    return mGameModes["default"]->getMusicScrollSpeed();
  }

  return -1;
}

int ThemeReader::getTimerNumber()
{
  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->getTimerNumber() != -1) return mCurrentGamemode->getTimerNumber();
  }
  if(mGameModes["default"]->getTimerNumber() != -1)
  {
    return mGameModes["default"]->getTimerNumber();
  }

  return 0;
}

QVector<QStringList> ThemeReader::getLayers()
{
  if(mCurrentGamemode != nullptr)
  {
    QVector<QStringList> layers = mCurrentGamemode->getLayers();

    if(layers.count() != 0) return layers;
  }

  QVector<QStringList> layers = mGameModes["default"]->getLayers();
  if(layers.count() != 0) return layers;
  return {};
}

  QStringList ThemeReader::getLayerChildren(QString t_widget)
{
  QStringList l_returnData = {};
  if(mCurrentGamemode != nullptr)
  {
    QVector<QStringList> l_rawLayers = mCurrentGamemode->getLayers();

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

  QVector<QStringList> l_rawLayers = mGameModes["default"]->getLayers();

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

  if(mCurrentGamemode != nullptr && mCurrentGamemode != mGameModes["default"])
  {
    return_data = mCurrentGamemode->getTabs();
    if(return_data.length() > 0) return return_data;
  }
  return_data = mGameModes["default"]->getTabs();
  return return_data;
}

bool ThemeReader::getConfigBool(QString boolValue)
{
  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->containsSettingBool(boolValue)) return mCurrentGamemode->getSettingBool(boolValue);
  }
  if(mGameModes["default"]->containsSettingBool(boolValue))
  {
    return mGameModes["default"]->getSettingBool(boolValue);
  }

  return false;
}

QString ThemeReader::getSoundName(QString soundName)
{
  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->containsSoundName(soundName)) return mCurrentGamemode->getSoundName(soundName);
  }
  if(mGameModes["default"]->containsSoundName(soundName))
  {
    return mGameModes["default"]->getSoundName(soundName);
  }

  return "";
}

QVector<QStringList> ThemeReader::getHighlights()
{
  QVector<QStringList> f_vec;
  QHash<QString, dialogueHighlights> returnValue = mGameModes["default"]->getHighlightColors(); ;
  if(mCurrentGamemode != nullptr && mCurrentGamemode != mGameModes["default"])
  {
    QHashIterator<QString, dialogueHighlights> i(mCurrentGamemode->getHighlightColors());
    while (i.hasNext())
    {
      i.next();
      returnValue[i.key()].chars = i.value().chars;
      returnValue[i.key()].color = i.value().color;
      returnValue[i.key()].keepCharacters = i.value().keepCharacters;
    }
  }
  QHashIterator<QString, dialogueHighlights> themeHighlights(returnValue);
  while(themeHighlights.hasNext())
  {
    themeHighlights.next();
    f_vec.append({themeHighlights.value().chars, themeHighlights.value().color, themeHighlights.value().keepCharacters ? "1" : "0"});
  }

  return f_vec;

}

QMap<QString, DR::ColorInfo> ThemeReader::getTextColors()
{
  QMap<QString, DR::ColorInfo> returnValue = mGameModes["default"]->getTextColors();
  if(mCurrentGamemode != nullptr && mCurrentGamemode != mGameModes["default"])
  {
    QMapIterator<QString, DR::ColorInfo> i(mCurrentGamemode->getTextColors());
    while (i.hasNext())
    {
      i.next();
      returnValue[i.key()].code = i.value().code;
    }
  }
  return returnValue;
}

bool ThemeReader::pixmapExists(QString path)
{
  if(mCurrentGamemode->mThemeImages.contains(path))
  {
    return true;
  }
  else if(mGameModes.contains("default"))
  {
    if(mGameModes["default"]->mThemeImages.contains(path))
    {
      return true;
    }
  }
  return false;
}

AOPixmap ThemeReader::getPixmap(QString path)
{
  if(mCurrentGamemode->mThemeImages.contains(path))
  {
    return mCurrentGamemode->mThemeImages[path];
  }
  else if(mGameModes.contains("default"))
  {
    if(mGameModes["default"]->mThemeImages.contains(path))
    {
      return mGameModes["default"]->mThemeImages[path];
    }
  }
  return AOPixmap();
}

widgetFontStruct ThemeReader::getFont(ThemeSceneType sceneType, QString element)
{
  widgetFontStruct return_value;
  double resize = ThemeManager::get().getResize();

  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->containsWidgetFont(sceneType, element))
    {
      return_value = mCurrentGamemode->getWidgetFont(sceneType, element);
      return_value.size = (int)((double)return_value.size * resize);
      return return_value;
    }
  }

  if(mGameModes["default"]->containsWidgetFont(sceneType, element))
  {
    return_value = mGameModes["default"]->getWidgetFont(sceneType, element);
  }
  return_value.size = (int)((double)return_value.size * resize);

  return return_value;
}

pos_size_type ThemeReader::getWidgetPosition(ThemeSceneType sceneType, QString element)
{
  pos_size_type return_value;

  double resize = ThemeManager::get().getResize();

  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->containsWidgetPosition(sceneType, element))
    {
      return_value = mCurrentGamemode->getWidgetPosition(sceneType, element);

      return_value.x = (int)((double)return_value.x * resize);
      return_value.y = (int)((double)return_value.y * resize);
      return_value.width = (int)((double)return_value.width * resize);
      return_value.height = (int)((double)return_value.height * resize);

      return return_value;
    }
  }

  if(mGameModes["default"]->containsWidgetPosition(sceneType, element))
  {
    return_value = mGameModes["default"]->getWidgetPosition(sceneType, element);
  }

  return_value.x = (int)((double)return_value.x * resize);
  return_value.y = (int)((double)return_value.y * resize);
  return_value.width = (int)((double)return_value.width * resize);
  return_value.height = (int)((double)return_value.height * resize);

  return return_value;
}

widgetFontStruct ThemeReader::getPairingFont(QString element, QString position)
{
  widgetFontStruct return_value;

  double resize = ThemeManager::get().getResize();
  QString name_alignment = element + "_" + position;

  if(mCurrentGamemode != nullptr)
  {
    if(mCurrentGamemode->containsWidgetFont(COURTROOM, name_alignment))
    {
      return_value = mCurrentGamemode->getWidgetFont(COURTROOM, name_alignment);
      return_value.size = (int)((double)return_value.size * resize);
      return return_value;
    }
    else if(mCurrentGamemode->containsWidgetFont(COURTROOM, element))
    {
      return_value = mCurrentGamemode->getWidgetFont(COURTROOM, element);
      return_value.size = (int)((double)return_value.size * resize);
      return return_value;
    }
  }


  if(mGameModes.contains("default"))
  {
    if(mGameModes["default"]->containsWidgetFont(COURTROOM, name_alignment)) return_value = mGameModes["default"]->getWidgetFont(COURTROOM, name_alignment);
    else if(mGameModes["default"]->containsWidgetFont(COURTROOM, element))
    {
      return_value = mGameModes["default"]->getWidgetFont(COURTROOM, element);
    }
  }

  return_value.size = (int)((double)return_value.size * resize);
  return return_value;

  return return_value;
}


