#include "area_menu.h"
#include "modules/theme/thememanager.h"
#include "courtroom.h"

#include <QListWidget>

AreaMenu::AreaMenu(QWidget *parent) : QMenu(parent)
{
  p_LockAction = addAction(tr("Lock Passage to Area"));
  p_PeekAction = addAction(tr("Peek into Area"));

  connect(p_LockAction, &QAction::triggered, this, &AreaMenu::OnLockTriggered);
  connect(p_PeekAction, &QAction::triggered, this, &AreaMenu::OnPeekTriggered);
}

void AreaMenu::OnMenuRequested(QPoint p_point)
{
  QListWidget *areaList = ThemeManager::get().GetWidgetType<QListWidget>("area_list");

  QListWidgetItem *l_item = areaList->currentItem();

  if (l_item)
  {
    const QString l_Area = l_item->text();
    if(l_Area.contains('-'))
    {
      bool l_result = false;
      m_TargetArea = l_Area.split('-').at(0).toInt(&l_result);

      if(!l_result)
      {
        m_TargetArea = -1;
        return;
      }
    }
  }

  const QPoint l_global_point = areaList->viewport()->mapToGlobal(p_point);
  popup(l_global_point);
}

void AreaMenu::OnLockTriggered()
{
  if(m_TargetArea < 0) return;
  AOApplication::getInstance()->get_courtroom()->send_ooc_packet({"/bilock " + QString::number(m_TargetArea)});
}

void AreaMenu::OnPeekTriggered()
{
  if(m_TargetArea < 0) return;
  AOApplication::getInstance()->get_courtroom()->send_ooc_packet({"/peek " + QString::number(m_TargetArea)});
}
