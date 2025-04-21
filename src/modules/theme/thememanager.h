#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <AOButton.h>
#include <QHash>
#include <aoimagedisplay.h>

#include <modules/theme/widgets/dro_combo_box.h>
#include <modules/theme/widgets/dro_line_edit.h>
#include "modules/json/theme_reader.h"



class ThemeManager
{
public:
  ThemeManager(const ThemeManager&) = delete;

  static ThemeManager& get()
  {
    return s_Instance;
  }

  //Tabs
  void createTabParent();
  void execLayerTabs();
  void resetSelectedTabs();
  void toggleTab(QString t_tabName, QString t_tabGroup);
  void detatchTab(QString t_tabName);
  QVector <ThemeTabInfo> getTabsInfo();


  void execRemoveWidget(QString t_name);

  void loadTheme(QString theme_name);
  void LoadGamemode(QString gamemode);

  void setWidgetPosition(QWidget *t_widget, int x, int y);
  void setWidgetDimensions(QWidget *t_widget, int t_width, int t_height);

  //Widgets
  void SetWidgetNames(QHash<QString, QWidget *> t_WidgetNames);
  void addWidgetName(QString t_widgetName, QWidget *t_widget);

  //Tabs


  //Data Management
  bool getConfigBool(QString value);
  bool getReloadPending();
  void toggleReload();

  //Widget Management
  pos_size_type resizePosition(pos_size_type t_position, double t_scale);

  void setResize(double size);
  double getResize();
  void setViewporResize(double size);
  double getViewporResize();

  void addButton(QString name, AOButton* button);
  void refreshButtons();

  void addLineEdit(QString name, DROLineEdit* lineEdit);
  void addComboBox(QString name, DROComboBox* lineEdit);
  void refreshLineEdit();
  void refreshComboBox();

  void setCourtroomBackground(AOImageDisplay *t_background);

  QWidget *getWidget(QString name);
  AOButton *GetButton(QString t_name);

  ThemeReader mCurrentThemeReader = ThemeReader();

private:
  ThemeManager() {}
  static ThemeManager s_Instance;

  double mClientResize = 1;

  double mViewportResize = 1;


  QMap<QString, QWidget *> waTabWidgets = {};

  QHash<QString, QWidget *> m_WidgetNames = {};
  QHash<QString, AOButton*> mButtonWidgets { };
  QHash<QString, DROLineEdit*> mLineEditWidgets { };
  QHash<QString, DROComboBox*> mComboBoxWidgets { };

  bool mRequiresReload = true;
  AOImageDisplay *wCourtroomBackground = nullptr;
};

#endif // THEMEMANAGER_H
