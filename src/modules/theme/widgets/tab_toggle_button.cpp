#include "tab_toggle_button.h"

#include <modules/theme/thememanager.h>

#include <QMenu>

TabToggleButton::TabToggleButton(QWidget *parent, AOApplication *p_ao_app) : AOButton(parent, p_ao_app)
{

  this->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this ,&QWidget::customContextMenuRequested, this, &TabToggleButton::displayContextMenu);
  connect(this, &QAbstractButton::clicked, this, &TabToggleButton::on_clicked);
}

void TabToggleButton::refreshButtonStatus()
{
  if(mTabActive)
  {
    set_image(mTabName + "_toggle_active.png");
    return;
  }
  set_image(mTabName + "_toggle.png");
}

void TabToggleButton::setTabName(QString t_name)
{
  mTabName = t_name;
  refreshButtonStatus();
}

void TabToggleButton::setTabGroup(QString t_name)
{
  mTabGroup = t_name;
}

void TabToggleButton::setActiveStatus(bool t_isActive)
{
  if(mTabActive == t_isActive) return;
  mTabActive = t_isActive;
  refreshButtonStatus();
}

void TabToggleButton::on_clicked()
{
  if(mIsDetatched) return;
  ThemeManager::get().toggleTab(mTabName, mTabGroup);
}

void TabToggleButton::displayContextMenu(QPoint t_position)
{
  QMenu *l_menu = new QMenu(this);
  QAction *l_detatchTab = new QAction("Detach from Window");
  QObject::connect(l_detatchTab, &QAction::triggered, [this](){detatchTab();});
  l_menu->addAction(l_detatchTab);
  l_menu->popup(this->mapToGlobal(t_position));
}

void TabToggleButton::detatchTab()
{
  if(mIsDetatched) return;
  ThemeManager::get().detatchTab(mTabName);
  mIsDetatched = true;
}
