#include "thememanager.h"
#include "courtroom.h"
#include "drtheme.h"
#include "aoapplication.h"
#include "dro/interface/widgets/tab_toggle_button.h"
#include "dro/interface/widgets/rp_widget.h"
#include "dro/system/theme_scripting.h"
#include "dro/interface/courtroom_layout.h"


ThemeManager ThemeManager::s_Instance;

void ThemeManager::ResetWidgetLists()
{
  m_TabWidgets.clear();
  m_TabDeletionQueue.clear();
  m_WidgetNames.clear();
  mButtonWidgets.clear();
  mLineEditWidgets.clear();
  mComboBoxWidgets.clear();
  m_DetatchedTabList.clear();
}

void ThemeManager::createTabParent()
{
  m_TabDeletionQueue = m_TabWidgets;
  m_TabWidgets = {};
  for(QString oldTabs : m_TabDeletionQueue.keys())
  {
    courtroom::tabs::deleteToggle(oldTabs);
  }

  for(ThemeTabInfo r_tabInfo : ThemeManager::get().getTabsInfo())
  {
    QWidget *l_courtroom = getWidget("courtroom");

    QString l_panelName = r_tabInfo.m_Name + "_panel";

    QWidget *tabParent = l_courtroom;
    if(m_DetatchedTabList.contains(l_panelName))
    {
      tabParent = nullptr;
    }
    RPWidget *l_newTab = new RPWidget(l_panelName, tabParent);


    QString l_buttonName = r_tabInfo.m_Name + "_toggle";

    pos_size_type l_panelPosition = mCurrentThemeReader.GetWidgetTransform(SceneType_Courtroom, l_panelName);
    pos_size_type l_buttonDimensions = mCurrentThemeReader.GetWidgetTransform(SceneType_Courtroom, l_buttonName);



    if(m_DetatchedTabList.contains(l_panelName) && m_TabDeletionQueue.contains(l_panelName))
    {
      l_newTab->move(m_TabDeletionQueue[l_panelName]->pos().x(), m_TabDeletionQueue[l_panelName]->pos().y());
    }
    else
    {
      l_newTab->move(l_panelPosition.x, l_panelPosition.y);
    }

    l_newTab->resize(l_panelPosition.width, l_panelPosition.height);
    l_newTab->setBackgroundImage(r_tabInfo.m_Name + "_panel");
    l_newTab->setDragable(r_tabInfo.m_DragEnabled);

    addWidgetName(l_panelName, l_newTab);

    if(m_TabWidgets.contains(r_tabInfo.m_Name))delete m_TabWidgets[r_tabInfo.m_Name];

    addWidgetName(l_panelName, l_newTab);
    m_TabWidgets[r_tabInfo.m_Name] = l_newTab;

    TabToggleButton *l_newButton = new TabToggleButton(l_courtroom, AOApplication::getInstance());
    l_newButton->setTabName(r_tabInfo.m_Name);
    l_newButton->setTabGroup(r_tabInfo.m_Group);
    l_newButton->show();


    l_newButton->move(l_buttonDimensions.x, l_buttonDimensions.y);
    l_newButton->resize(l_buttonDimensions.width, l_buttonDimensions.height);

    addWidgetName(l_buttonName,  l_newButton);
  };


  LuaBridge::LuaEventCall("OnTogglesCreated");
}

void ThemeManager::execLayerTabs()
{
  for(ThemeTabInfo r_tabInfo : ThemeManager::get().getTabsInfo())
  {
    if(m_TabWidgets.contains(r_tabInfo.m_Name))
    {
      for(QString r_WidgetName : r_tabInfo.m_WidgetContents)
      {
        QWidget *l_ChildWidget = getWidget(r_WidgetName);
        if(l_ChildWidget != nullptr)
        {
          l_ChildWidget->setParent(m_TabWidgets[r_tabInfo.m_Name]);
        }
      }
      m_TabWidgets[r_tabInfo.m_Name]->raise();
      m_TabWidgets[r_tabInfo.m_Name]->hide();
    }
  };

  QMap<QString, QWidget *>::iterator it;

  for (it = m_TabDeletionQueue.begin(); it != m_TabDeletionQueue.end(); ++it)
  {
    Courtroom* target = AOApplication::getInstance()->get_courtroom();
    QWidget *value = it.value();
    if(target != nullptr)
    {
      const auto children = value->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
      for (QWidget *child : children)
      {
        child->setParent(target);
      }
    }
    delete value;
  }
}

void ThemeManager::resetSelectedTabs()
{
  QStringList l_resetTabs = {};
  for(ThemeTabInfo r_tabInfo : ThemeManager::get().getTabsInfo())
  {
    if(l_resetTabs.contains(r_tabInfo.m_Group)) continue;
    toggleTab(r_tabInfo.m_Name, r_tabInfo.m_Group);
    l_resetTabs.append(r_tabInfo.m_Group);
  }
}

void ThemeManager::toggleTab(const QString& name, const QString& group)
{
  QVector<ThemeTabInfo> tabsInfo = ThemeManager::get().getTabsInfo();
  const ThemeTabInfo* selectedTab = nullptr;

  for (const ThemeTabInfo& tabInfo : tabsInfo)
  {
    if (tabInfo.m_Name == name && tabInfo.m_Group == group)
    {
      selectedTab = &tabInfo;
      break;
    }
  }

  if (!selectedTab) return;

  const bool isToggle = selectedTab->m_ToggleEnabled;

  for (const ThemeTabInfo& tabInfo : tabsInfo)
  {
    if (tabInfo.m_Group != group) continue;

    const QString buttonName = tabInfo.m_Name + "_toggle";
    const QString panelName = tabInfo.m_Name + "_panel";

    bool isCurrent = (tabInfo.m_Name == name);

    if (!m_WidgetNames.contains(panelName)) continue;

    if (m_WidgetNames.contains(buttonName))
    {
      if (auto* tabButton = dynamic_cast<TabToggleButton*>(getWidget(buttonName))) tabButton->setActiveStatus(isCurrent);
    }

    if (isCurrent)
    {
      getWidget(panelName)->show();

      const QString bgImage = AOApplication::getInstance()->find_theme_asset_path("courtroombackground_" + name + ".png");
      if (!bgImage.isEmpty())
        wCourtroomBackground->set_theme_image("courtroombackground_" + name + ".png");
      else
        wCourtroomBackground->set_theme_image("courtroombackground.png");
    }
    else if (!isToggle)
    {
      if(!tabInfo.m_ToggleEnabled)
      {
        if (m_DetatchedTabList.contains(panelName)) continue;
        getWidget(panelName)->hide();
      }
    }
  }

}

void ThemeManager::detatchTab(QString t_tabName)
{
  QString l_panelName = t_tabName + "_panel";
  QWidget *l_widget = getWidget(l_panelName);

  if(l_widget != nullptr)
  {
    l_widget->setParent(nullptr);
    l_widget->move(40, 40);
    l_widget->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    l_widget->show();

    m_DetatchedTabList[l_panelName] = {40, 40};
  }
}

void ThemeManager::execRemoveWidget(QString t_name)
{
  if(m_WidgetNames.contains(t_name)) m_WidgetNames.remove(t_name);
}


void ThemeManager::loadTheme(QString theme_name)
{
  if(mRequiresReload)
  {
    mCurrentThemeReader.LoadTheme(theme_name);
    mRequiresReload = false;
  }
}

void ThemeManager::LoadGamemode(QString gamemode)
{
  mCurrentThemeReader.SetGamemode(gamemode);
}

void ThemeManager::setWidgetPosition(QWidget *t_widget, int t_x, int t_y)
{
  int l_PositionX = static_cast<int>(t_x * mClientResize);
  int l_PositionY = static_cast<int>(t_y * mClientResize);

  t_widget->move(l_PositionX, l_PositionY);
}

void ThemeManager::setWidgetDimensions(QWidget *t_widget, int t_width, int t_height)
{
  int l_PositionWidth = static_cast<int>(t_width * mClientResize);
  int l_PositionHeight = static_cast<int>(t_height * mClientResize);

  t_widget->resize(l_PositionWidth, l_PositionHeight);
}

void ThemeManager::AssignDimensions(QWidget *t_widget, QString t_name, ThemeSceneType t_scene)
{
  pos_size_type lPositionData = mCurrentThemeReader.GetWidgetTransform(t_scene, t_name);
  lPositionData.width = static_cast<int>(lPositionData.width);
  lPositionData.height = static_cast<int>(lPositionData.height);
  lPositionData.x = static_cast<int>(lPositionData.x);
  lPositionData.y = static_cast<int>(lPositionData.y);

  t_widget->move(lPositionData.x, lPositionData.y);
  t_widget->resize(lPositionData.width, lPositionData.height);
}

void ThemeManager::SetWidgetNames(QHash<QString, QWidget *> t_WidgetNames)
{
  m_WidgetNames = t_WidgetNames;
}

void ThemeManager::addWidgetName(QString t_widgetName, QWidget *t_widget)
{
  courtroom::layout::addWidget(t_widgetName, t_widget);
  m_WidgetNames[t_widgetName] = t_widget;
}

QVector<ThemeTabInfo> ThemeManager::getTabsInfo()
{
  return ThemeManager::get().mCurrentThemeReader.getTabs();
}

bool ThemeManager::getConfigBool(QString value)
{
  return mCurrentThemeReader.GetConfigBool(value);
}

bool ThemeManager::getReloadPending()
{
  return mRequiresReload;
}

void ThemeManager::toggleReload()
{
  mRequiresReload = true;
}

pos_size_type ThemeManager::resizePosition(pos_size_type t_position, double t_scale)
{
  t_position.x = static_cast<int>(t_position.x * t_scale);
  t_position.y = static_cast<int>(t_position.y * t_scale);
  t_position.width = static_cast<int>(t_position.width * t_scale);
  t_position.height = static_cast<int>(t_position.height * t_scale);

  return t_position;
}

void ThemeManager::setResize(double size)
{
  mClientResize = size;
}

double ThemeManager::getResize()
{
  return mClientResize;
}

void ThemeManager::setViewporResize(double size)
{
  mViewportResize = size;
}

double ThemeManager::getViewporResize()
{
  return mViewportResize;
}

void ThemeManager::addComboBox(QString name, RPComboBox *comboBox)
{
  mComboBoxWidgets[name] = comboBox;
}

void ThemeManager::refreshComboBox()
{
  for(RPComboBox* comboBox : mComboBoxWidgets)
  {
    comboBox->refreshPosition();
    comboBox->refreshCSS();
    comboBox->show();
  }
}

void ThemeManager::setCourtroomBackground(AOImageDisplay *t_background)
{
  wCourtroomBackground = t_background;
}

QWidget *ThemeManager::getWidget(QString name)
{
  if(m_WidgetNames.contains(name)) return m_WidgetNames[name];
  return nullptr;
}

RPButton *ThemeManager::GetButton(QString t_name)
{
  return dynamic_cast<RPButton*>(ThemeManager::get().getWidget(t_name));
}
