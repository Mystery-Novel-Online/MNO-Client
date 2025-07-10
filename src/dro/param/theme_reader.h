#ifndef THEMEREADER_H
#define THEMEREADER_H

#include "theme_mode_reader.h"

class ThemeReader
{
public:
  ThemeReader();

  void LoadTheme(QString themeName);
  void SetGamemode(QString gamemode);
  void SetTimeOfDay(QString time);

  //IC Chatlog Font
  QColor getChatlogColor(QString type);
  bool getChatlogBool(QString t_type);

  QStringList getThemeDirOrder();

  bool IsPixmapExist(QString path);
  AOPixmap GetCachedPixmap(QString path);

  QVector<QStringList> GetLayers();
  QStringList GetLayerChildren(QString t_widget);

  bool GetConfigBool(QString boolValue);
  QString GetConfigSoundName(QString soundName);

  int getMusicScrollSpeed();
  int getTimerNumber();

  QVector<ThemeTabInfo> getTabs();

  pos_size_type GetWidgetTransform(ThemeSceneType sceneType, QString element);
  QVector2D GetWidgetSpacing(QString widget_name);

  widgetFontStruct GetFontData(ThemeSceneType sceneType, QString element);
  widgetFontStruct GetFontDataPairing(QString element, QString position);

  QVector<QStringList> GetColorsHighlights();
  QMap<QString, DR::ColorInfo> GetColorsDefault();

  QStringList GetSceneWidgetNames(ThemeSceneType t_sceneType)
  {
    QStringList l_widgetNames = {};
    QVector<ThemeScene *> l_scenes = RetrieveSceneOrder(t_sceneType, true);
    for(ThemeScene * r_scene : l_scenes)
    {
      if(r_scene != nullptr)
      {
        l_widgetNames.append(r_scene->getWidgetNames());
      }
    }
    return l_widgetNames;
  }

  WidgetThemeData * GetWidgetData(ThemeSceneType t_sceneType, QString t_name)
  {
    QVector<ThemeScene *> l_scenes = RetrieveSceneOrder(t_sceneType, false);
    for(ThemeScene * r_scene : l_scenes)
    {
      if(r_scene != nullptr)
      {
        if(r_scene->containsWidget(t_name)) return r_scene->getWidgetData(t_name);
      }
    }
    return nullptr;
  }

private:
  QString m_GameModeCurrentName = "";
  ThemeModeReader* m_GameModeCurrent = nullptr;
  QHash<QString, ThemeModeReader*> m_GameModeCollection = {};

  ThemeModeReader* GetCurrentGamemode()
  {
    if(m_GameModeCurrent != nullptr) return m_GameModeCurrent;
    else return m_GameModeCollection["default"];
  };

  QVector<ThemeModeReader*> GetGamemodeOrder()
  {
    QVector<ThemeModeReader*> l_ReturnData;
    if(m_GameModeCurrent != nullptr && m_GameModeCurrentName != "default") l_ReturnData.append(m_GameModeCurrent);

    if(m_GameModeCollection.contains("default")) l_ReturnData.append(m_GameModeCollection["default"]);

    return l_ReturnData;
  };


  QVector<ThemeModuleReader *> RetrieveModuleOrder()
  {
    QVector<ThemeModuleReader *> l_returnData = {};

    if(m_GameModeCurrent != nullptr)
    {
      l_returnData.append(m_GameModeCurrent->getModuleLoadOrder());
    }

    if(m_GameModeCollection.contains("default"))
    {
      l_returnData.append(m_GameModeCollection["default"]->getModuleLoadOrder());
    }

    return l_returnData;

  }

  QVector<ThemeScene *> RetrieveSceneOrder(ThemeSceneType t_type, bool t_onlyFirst = false)
  {
    QVector<ThemeScene *> l_returnData = {};

    if(m_GameModeCurrent != nullptr)
    {
      l_returnData.append(m_GameModeCurrent->getSceneLoadOrder(t_type));
      if(l_returnData.count() != 0 && t_onlyFirst) return l_returnData;
    }

    if(m_GameModeCollection.contains("default"))
    {
      l_returnData.append(m_GameModeCollection["default"]->getSceneLoadOrder(t_type));
    }

    return l_returnData;

  }

  QString m_ThemeName = "";
  QString m_ThemeDirectory = "";
  QString m_TimeOfDay = "";
};

#endif // THEMEREADER_H
