#ifndef THEMEREADER_H
#define THEMEREADER_H

#include "theme_mode_reader.h"

class ThemeReader
{
public:
  ThemeReader();

  void LoadTheme(QString themeName);
  void SetGamemode(QString gamemode);
  void SetTime(QString time);

  ThemeModeReader* mCurrentGamemode = nullptr;

  QString mThemeName = "";
  QString mTime = "";
  QString mCurrentGamemodeName = "";
  QString mThemeDirectory = "";
  QHash<QString, ThemeModeReader*> mGameModes = {};


  //IC Chatlog Font
  QColor getChatlogColor(QString type);
  bool getChatlogBool(QString t_type);

  QStringList getThemeDirOrder();

  //Data Grabbing
  QVector2D getWidgetSpacing(QString widget_name);
  QVector<QStringList> getHighlights();
  int getMusicScrollSpeed();
  int getTimerNumber();
  QVector<QStringList> getLayers();
  QStringList getLayerChildren(QString t_widget);
  QVector<ThemeTabInfo> getTabs();
  QString getSoundName(QString soundName);
  QMap<QString, DR::ColorInfo> getTextColors();
  bool getConfigBool(QString boolValue);
  bool pixmapExists(QString path);
  AOPixmap getPixmap(QString path);
  widgetFontStruct getFont(ThemeSceneType sceneType, QString element);
  pos_size_type getWidgetPosition(ThemeSceneType sceneType, QString element);
  widgetFontStruct getPairingFont(QString element, QString position);
};

#endif // THEMEREADER_H
