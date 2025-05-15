#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QHash>
#include "dro/interface/widgets/image_display.h"
#include "dro/interface/widgets/rp_combo_box.h"
#include "dro/interface/widgets/rp_line_edit.h"
#include "dro/interface/widgets/rp_button.h"
#include "dro/param/theme_reader.h"

class ThemeManager
{
public:
  ThemeManager(const ThemeManager&) = delete;

  static ThemeManager& get()
  {
    return s_Instance;
  }

  void ResetWidgetLists();

  //Tabs
  void createTabParent();
  void execLayerTabs();
  void resetSelectedTabs();
  void toggleTab(const QString& name, const QString& group);
  void detatchTab(QString t_tabName);
  QVector <ThemeTabInfo> getTabsInfo();


  void execRemoveWidget(QString t_name);

  void loadTheme(QString theme_name);
  void LoadGamemode(QString gamemode);

  void setWidgetPosition(QWidget *t_widget, int x, int y);
  void setWidgetDimensions(QWidget *t_widget, int t_width, int t_height);


  void AssignDimensions(QWidget *t_widget, QString t_name, ThemeSceneType t_scene);

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

  void addComboBox(QString name, DROComboBox* lineEdit);
  void refreshComboBox();

  void setCourtroomBackground(AOImageDisplay *t_background);


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
  RPButton *GetButton(QString t_name);

  ThemeReader mCurrentThemeReader = ThemeReader();

private:
  ThemeManager() {}
  static ThemeManager s_Instance;

  double mClientResize = 1;

  double mViewportResize = 1;


  QMap<QString, QWidget *> m_TabWidgets = {};
  QMap<QString, QWidget *> m_TabDeletionQueue = {};

  QHash<QString, QWidget *> m_WidgetNames = {};
  QHash<QString, RPButton*> mButtonWidgets { };
  QHash<QString, DROLineEdit*> mLineEditWidgets { };
  QHash<QString, DROComboBox*> mComboBoxWidgets { };

  bool mRequiresReload = true;
  AOImageDisplay *wCourtroomBackground = nullptr;

  QMap<QString, QVector2D> m_DetatchedTabList = {};
};

#endif // THEMEMANAGER_H
