#ifndef THEMEMODEREADER_H
#define THEMEMODEREADER_H

#include "json_reader.h"
#include "modules/json/theme_module_reader.h"

#include <aopixmap.h>
#include <datatypes.h>


class ThemeModeReader : public JSONReader
{
public:
  ThemeModeReader(QString filePath);
  ~ThemeModeReader();
  void LoadTimeMode();
  void LoadModules();

  QStringList m_ModuleNames = {};

  ThemeModuleReader* m_GamemodeModule = nullptr;
  QHash<QString, ThemeModuleReader*> m_TimeModules = {};
  QHash<QString, QHash<QString, ThemeModuleReader*>> m_ThemeModules = {};



  QStringList getThemeDirOrder();

  QHash<QString, bool> mConfigToggles = {};

  int mMusicScrollSpeed = -1;
  int mTimerNumber = -1;

  QMap<QString, DR::ColorInfo> mTextColours = {};
  QHash<QString, QString> mSounds = {};

  void SetThemeJsonObject(QString category);

  void SetTime(QString time);


  ThemeModeReader* mCurrentTime = nullptr;
  QString mCurrentTimeName = "";



  QHash<QString, pos_size_type> mCourtroomWidgetPositions = {};
  QHash<QString, widgetFontStruct> mCourtroomWidgetFonts = {};
  QHash<QString, dialogueHighlights> mCourtroomFontHighlights = {};


  QHash<QString, pos_size_type> mLobbyWidgetPositions = {};
  QHash<QString, widgetFontStruct> mLobbyWidgetFonts = {};

  QHash<QString, QStringList> mTabWidgets = {};
  QMap<QString, QStringList> mWidgetLayers = {};


  QHash<QString, AOPixmap> mThemeImages = {};

  QHash<QString, QVector2D> mWidgetSpacings = {};

  QString mFilePath = "";


  QHash<QString, ThemeModeReader*> mTimeModes = {};

  //IC Chatlog
  QColor getChatlogColour(QString t_type);
  bool getChatlogBool(QString t_type);
  bool getContainsChatlogColour(QString t_type);
  bool getContainsChatlogBool(QString t_type);


  //Data Retrieval
  QVector<ThemeTabInfo> getTabs();
  QVector<QStringList> getLayers();
  QHash<QString, dialogueHighlights>getHighlightColors();
  QMap<QString, DR::ColorInfo>getTextColors();

  bool containsSoundName(QString setting);
  QString getSoundName(QString setting);

  //Data Retrieval
  bool containsSettingBool(QString setting);
  bool getSettingBool(QString setting);

  QVector2D getWidgetSpacing(QString widget_name);
  int getMusicScrollSpeed();
  int getTimerNumber();

  bool containsWidgetPosition(ThemeSceneType sceneType, QString name);
  bool containsCourtroomWidgetPosition(QString widget_name);
  bool containsLobbyWidgetPosition(QString widget_name);

  pos_size_type getWidgetPosition(ThemeSceneType sceneType, QString name);

  bool containsWidgetFont(ThemeSceneType sceneType, QString widget_name);
  widgetFontStruct getWidgetFont(ThemeSceneType t_sceneType, QString t_name);

  pos_size_type getWidgetDimensions(QVector<ThemeScene *> t_readOrder, QString t_name);

  QVector<ThemeScene *> getSceneLoadOrder(ThemeSceneType t_type);
  QVector<ThemeModuleReader *>getModuleLoadOrder();
};

#endif // THEMEMODEREADER_H
