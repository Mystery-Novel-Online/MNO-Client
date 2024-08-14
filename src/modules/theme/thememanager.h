#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QHash>
#include <aobutton.h>
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
  void CreateTabWidgets();
  void ParentWidgetsToTabs();

  void ResetTabSelection();
  void ToggleTab(QString t_tabName, QString t_tabGroup);
  void DetatchTab(QString t_tabName);
  QVector <ThemeTabInfo> getTabsInfo();


  void loadTheme(QString theme_name);
  void LoadGamemode(QString gamemode);


  void AdjustWidgetTransform(QWidget *t_widget, int x, int y);
  void AdjustWidgetDimensions(QWidget *t_widget, int t_width, int t_height);
  void AutoAdjustWidgetDimensions(QWidget *t_widget, QString t_name, ThemeSceneType t_scene);
  void AutoAdjustWidgetDimensions(QWidget *t_widget, QString t_name, ThemeSceneType t_scene, double l_ManualResize);

  //Widgets
  void RegisterWidgetGenericBulk(QHash<QString, QWidget *> t_WidgetNames);
  void RegisterWidgetGeneric(QString t_widgetName, QWidget *t_widget);

  void RegisterWidgetButton(QString name, AOButton* button);
  void RegisterWidgetLineEdit(QString name, DROLineEdit* lineEdit);
  void RegisterWidgetComboBox(QString name, DROComboBox* lineEdit);

  void RegisterCourtroomBackground(AOImageDisplay *t_background);

  void UnregisterWidget(QString t_name);
  void UnregisterWidgetsAll();


  //Data Management
  bool GetThemeConfigBool(QString value);

  void QueueFullReload();

  bool GetFullReloadQueued()
  {
    return m_FullReloadQueued;
  };


  //Widget Management
  pos_size_type resizePosition(pos_size_type t_position, double t_scale);

  void SetResizeClient(double size);
  void SetResizeViewport(double size);
  double GetResizeClient();
  double GetResizeViewport();

  void RefreshWidgetsLineEdit();
  void RefreshWidgetsComboBox();
  void RefreshWidgetsButton();


  template<typename T>
  T* GetWidgetType(QString t_name)
  {
    QWidget *mReturnWidget = ThemeManager::get().getWidget(t_name);
    if (dynamic_cast<T*>(mReturnWidget) != nullptr)
    {
      T* l_return = dynamic_cast<T*>(mReturnWidget);
      return l_return;
    }

    return nullptr;
  }


  QWidget *getWidget(QString name);

  AOButton *CreateWidgetButton(ThemeSceneType t_scene, QString t_name, QString t_image = "", QString t_text = "", QWidget *t_parent = nullptr);
  AOImageDisplay *CreateWidgetImageDisplay(QString t_name, QString t_image, bool visible, QWidget *t_parent = nullptr);
  void CreateWidgetCombobox(QString t_name);

public:
  //This needs to be made private.
  ThemeReader mCurrentThemeReader = ThemeReader();

private:
  ThemeManager() {}
  static ThemeManager s_Instance;

  bool m_FullReloadQueued = true;

  double m_ResizeClient = 1;
  double m_ResizeViewport = 1;

  QMap<QString, QWidget *> m_TabWidgets = {};
  QMap<QString, QWidget *> m_TabDeletionQueue = {};

  QHash<QString, QWidget *> m_WidgetNames = {};
  QHash<QString, AOButton*> m_WidgetsButtons { };
  QHash<QString, DROLineEdit*> m_WidgetsLineEdit { };
  QHash<QString, DROComboBox*> m_WidgetsComboBox { };
  AOImageDisplay *m_WidgetCourtroomBackground = nullptr;
};

#endif // THEMEMANAGER_H
