#include "char_menu.h"
#include "modules/theme/thememanager.h"
#include "courtroom.h"

#include <QListWidget>

CharMenu::CharMenu(QWidget *parent) : QMenu(parent)
{
  p_UpdateCharacterFilesAction = addAction(tr("Update Character Files Link"));

  connect(p_UpdateCharacterFilesAction, &QAction::triggered, this, &CharMenu::OnUpdateCharacterFilesTriggered);
}

void CharMenu::OnMenuRequested(QPoint p_point)
{
  QPushButton *changeCharacterButton = ThemeManager::get().GetWidgetType<QPushButton>("change_character");
  const QPoint l_global_point = changeCharacterButton->mapToGlobal(p_point);
  popup(l_global_point);
}

void CharMenu::OnUpdateCharacterFilesTriggered()
{
  //TODO
}
