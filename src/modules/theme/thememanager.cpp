#include "thememanager.h"
#include "courtroom.h"
#include "drtheme.h"
#include "aoapplication.h"
#include "widgets/tab_toggle_button.h"
#include "../widgets/tabgroupingwidget.h"
#include <QComboBox>

ThemeManager ThemeManager::s_Instance;

void ThemeManager::CreateTabWidgets()
{
  m_TabDeletionQueue = m_TabWidgets;
  m_TabWidgets = {};

  for(ThemeTabInfo r_tabInfo : ThemeManager::get().getTabsInfo())
  {
    QWidget *l_courtroom = getWidget("courtroom");
    TabGroupingWidget *l_newTab = new TabGroupingWidget(l_courtroom);


    QString l_buttonName = r_tabInfo.m_Name + "_toggle";
    QString l_panelName = r_tabInfo.m_Name + "_panel";

    pos_size_type l_panelPosition = mCurrentThemeReader.GetWidgetTransform(SceneTypeCourtroom, l_panelName);
    pos_size_type l_buttonDimensions = mCurrentThemeReader.GetWidgetTransform(SceneTypeCourtroom, l_buttonName);


    l_newTab->move(l_panelPosition.x, l_panelPosition.y);
    l_newTab->resize(l_panelPosition.width, l_panelPosition.height);
    l_newTab->setBackgroundImage(r_tabInfo.m_Name);

    RegisterWidgetGeneric(l_panelName, l_newTab);

    if(m_TabWidgets.contains(r_tabInfo.m_Name))delete m_TabWidgets[r_tabInfo.m_Name];

    m_TabWidgets[r_tabInfo.m_Name] = l_newTab;

    TabToggleButton *l_newButton = new TabToggleButton(l_courtroom, AOApplication::getInstance());
    l_newButton->setTabName(r_tabInfo.m_Name);
    l_newButton->setTabGroup(r_tabInfo.m_Group);
    l_newButton->show();

    AdjustWidgetDimensions(l_newButton, l_buttonDimensions.width, l_buttonDimensions.height);
    AdjustWidgetTransform(l_newButton, l_buttonDimensions.x, l_buttonDimensions.y);

    RegisterWidgetGeneric(l_buttonName,  l_newButton);
    RegisterWidgetButton(l_buttonName, l_newButton);
  };

}

void ThemeManager::ParentWidgetsToTabs()
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
    QWidget *value = it.value();
    delete value;
  }

}

void ThemeManager::ResetTabSelection()
{
  QStringList l_resetTabs = {};
  for(ThemeTabInfo r_tabInfo : ThemeManager::get().getTabsInfo())
  {
    if(l_resetTabs.contains(r_tabInfo.m_Group)) continue;
    ToggleTab(r_tabInfo.m_Name, r_tabInfo.m_Group);
    l_resetTabs.append(r_tabInfo.m_Group);
  }
}

void ThemeManager::ToggleTab(QString t_tabName, QString t_tabGroup)
{
  for(ThemeTabInfo r_tabInfo : ThemeManager::get().getTabsInfo())
  {

    QString l_buttonName = r_tabInfo.m_Name + "_toggle";
    QString l_tabPanel = r_tabInfo.m_Name + "_panel";

    if(r_tabInfo.m_Group != t_tabGroup) continue;
    if(!m_WidgetNames.contains(l_tabPanel)) continue;

    if(r_tabInfo.m_Name == t_tabName)
    {
      if(m_WidgetNames.contains(l_buttonName))
      {
        TabToggleButton* l_tabButton = dynamic_cast<TabToggleButton*>(getWidget(l_buttonName));
        l_tabButton->setActiveStatus(true);
      }

      getWidget(l_tabPanel)->show();

      QString l_bg_image = AOApplication::getInstance()->find_theme_asset_path("courtroombackground_" + t_tabName + ".png");
      if (!l_bg_image.isEmpty())
      {
        m_WidgetCourtroomBackground->set_theme_image("courtroombackground_" + t_tabName + ".png");
      }
      else
      {
        m_WidgetCourtroomBackground->set_theme_image("courtroombackground.png");
      }


    }
    else
    {
      if(m_WidgetNames.contains(l_buttonName))
      {
        TabToggleButton* l_tabButton = dynamic_cast<TabToggleButton*>(getWidget(l_buttonName));
        l_tabButton->setActiveStatus(false);
      }
      getWidget(l_tabPanel)->hide();
    }

  }

}

void ThemeManager::DetatchTab(QString t_tabName)
{
  QString l_panelName = t_tabName + "_panel";
  QWidget *l_widget = getWidget(l_panelName);

  if(l_widget != nullptr)
  {
    l_widget->setParent(nullptr);
    l_widget->move(40, 40);
    l_widget->setWindowFlag(Qt::WindowStaysOnTopHint, true);
    l_widget->show();

  }
}

void ThemeManager::UnregisterWidget(QString t_name)
{
  if(m_WidgetNames.contains(t_name)) m_WidgetNames.remove(t_name);
}


void ThemeManager::loadTheme(QString theme_name)
{
  if(m_FullReloadQueued)
  {
    mCurrentThemeReader.LoadTheme(theme_name);
    m_FullReloadQueued = false;
  }
}

void ThemeManager::LoadGamemode(QString gamemode)
{
  mCurrentThemeReader.SetGamemode(gamemode);
}

void ThemeManager::AdjustWidgetTransform(QWidget *t_widget, int t_x, int t_y)
{
  int l_PositionX = static_cast<int>(t_x * m_ResizeClient);
  int l_PositionY = static_cast<int>(t_y * m_ResizeClient);

  t_widget->move(l_PositionX, l_PositionY);
}

void ThemeManager::AdjustWidgetDimensions(QWidget *t_widget, int t_width, int t_height)
{
  int l_PositionWidth = static_cast<int>(t_width * m_ResizeClient);
  int l_PositionHeight = static_cast<int>(t_height * m_ResizeClient);

  t_widget->resize(l_PositionWidth, l_PositionHeight);
}

void ThemeManager::AutoAdjustWidgetDimensions(QWidget *t_widget, QString t_name, ThemeSceneType t_scene)
{
  pos_size_type lPositionData = mCurrentThemeReader.GetWidgetTransform(t_scene, t_name);
  lPositionData.width = static_cast<int>(lPositionData.width * m_ResizeClient);
  lPositionData.height = static_cast<int>(lPositionData.height * m_ResizeClient);
  lPositionData.x = static_cast<int>(lPositionData.x * m_ResizeClient);
  lPositionData.y = static_cast<int>(lPositionData.y * m_ResizeClient);

  t_widget->move(lPositionData.x, lPositionData.y);
  t_widget->resize(lPositionData.width, lPositionData.height);
}

void ThemeManager::AutoAdjustWidgetDimensions(QWidget *t_widget, QString t_name, ThemeSceneType t_scene, double l_ManualResize)
{
  pos_size_type lPositionData = mCurrentThemeReader.GetWidgetTransform(t_scene, t_name);
  lPositionData.width = static_cast<int>(lPositionData.width * l_ManualResize);
  lPositionData.height = static_cast<int>(lPositionData.height * l_ManualResize);
  lPositionData.x = static_cast<int>(lPositionData.x * l_ManualResize);
  lPositionData.y = static_cast<int>(lPositionData.y * l_ManualResize);

  t_widget->move(lPositionData.x, lPositionData.y);
  t_widget->resize(lPositionData.width, lPositionData.height);
}

void ThemeManager::RegisterWidgetGenericBulk(QHash<QString, QWidget *> t_WidgetNames)
{
  m_WidgetNames = t_WidgetNames;
}

void ThemeManager::RegisterWidgetGeneric(QString t_widgetName, QWidget *t_widget)
{
  m_WidgetNames[t_widgetName] = t_widget;
}

QVector<ThemeTabInfo> ThemeManager::getTabsInfo()
{
  return ThemeManager::get().mCurrentThemeReader.getTabs();
}

bool ThemeManager::GetThemeConfigBool(QString value)
{
  return mCurrentThemeReader.GetConfigBool(value);
}

void ThemeManager::QueueFullReload()
{
  m_FullReloadQueued = true;
}

pos_size_type ThemeManager::resizePosition(pos_size_type t_position, double t_scale)
{
  t_position.x = static_cast<int>(t_position.x * t_scale);
  t_position.y = static_cast<int>(t_position.y * t_scale);
  t_position.width = static_cast<int>(t_position.width * t_scale);
  t_position.height = static_cast<int>(t_position.height * t_scale);

  return t_position;
}

void ThemeManager::SetResizeClient(double size)
{
  m_ResizeClient = size;
}

double ThemeManager::GetResizeClient()
{
  return m_ResizeClient;
}

void ThemeManager::SetResizeViewport(double size)
{
  m_ResizeViewport = size;
}

double ThemeManager::GetResizeViewport()
{
  return m_ResizeViewport;
}

void ThemeManager::RegisterWidgetButton(QString name, AOButton *button)
{
  m_WidgetsButtons[name] = button;
}

void ThemeManager:: RegisterWidgetLineEdit(QString name, DROLineEdit* lineEdit)
{
  m_WidgetsLineEdit[name] = lineEdit;
}

void ThemeManager::RefreshWidgetsButton()
{
  for(AOButton* button : m_WidgetsButtons)
  {
    button->set_theme_image();
    button->refresh_position();
    button->show();
  }
}

void ThemeManager::RefreshWidgetsLineEdit()
{
  for(DROLineEdit* lineEdit : m_WidgetsLineEdit)
  {
    lineEdit->refreshPosition();
    lineEdit->refreshCSS();
    lineEdit->show();
  }
}

void ThemeManager::RegisterWidgetComboBox(QString name, DROComboBox *comboBox)
{
  m_WidgetsComboBox[name] = comboBox;
}

void ThemeManager::RefreshWidgetsComboBox()
{
  for(DROComboBox* comboBox : m_WidgetsComboBox)
  {
    comboBox->refreshPosition();
    comboBox->refreshCSS();
    comboBox->show();
  }
}

void ThemeManager::RegisterCourtroomBackground(AOImageDisplay *t_background)
{
  m_WidgetCourtroomBackground = t_background;
}


QWidget *ThemeManager::getWidget(QString name)
{
  if(m_WidgetNames.contains(name)) return m_WidgetNames[name];
  return nullptr;
}

void ThemeManager::UnregisterWidgetsAll()
{
  m_TabWidgets.clear();
  m_WidgetNames.clear();
  m_WidgetsButtons.clear();
  m_WidgetsLineEdit.clear();
  m_WidgetsComboBox.clear();
}

AOButton *ThemeManager::CreateWidgetButton(ThemeSceneType t_scene, QString t_name, QString t_image, QString t_text, QWidget *t_parent)
{
  if(m_WidgetNames.contains(t_name))
  {
    delete m_WidgetNames[t_name];
    m_WidgetNames.remove(t_name);
  }

  AOButton *l_ButtonWidget = new AOButton(t_parent, AOApplication::getInstance());

  AutoAdjustWidgetDimensions(l_ButtonWidget, t_name, t_scene);

  l_ButtonWidget->set_theme_image(t_name, t_image, "courtroom", t_text);

  RegisterWidgetGeneric(t_name, l_ButtonWidget);

  l_ButtonWidget->show();

  m_WidgetsButtons[t_name] = l_ButtonWidget;

  return l_ButtonWidget;
}

AOImageDisplay *ThemeManager::CreateWidgetImageDisplay(QString t_name, QString t_image, bool visible, QWidget *t_parent)
{
  AOImageDisplay *l_image = new AOImageDisplay(t_parent, AOApplication::getInstance());
  AutoAdjustWidgetDimensions(l_image, t_name, SceneTypeReplays);
  RegisterWidgetGeneric(t_name, l_image);
  l_image->set_theme_image(AOApplication::getInstance()->current_theme->get_widget_image(t_name, t_image, "replays"));

  if(!visible) l_image->hide();
  else l_image->show();

  return l_image;
}

void ThemeManager::CreateWidgetCombobox(QString t_name)
{

}
