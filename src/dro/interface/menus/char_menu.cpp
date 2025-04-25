#include "char_menu.h"
#include "modules/theme/thememanager.h"
#include "courtroom.h"

#include <QInputDialog>
#include <QListWidget>

CharMenu::CharMenu(Courtroom *parent) : QMenu(parent), parentCourtroom(parent)
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
  bool userEnteredInput;
  QString filesLink = QInputDialog::getText(this,
                                       tr("Update Character Files"),
                                       tr("Enter link:"),
                                       QLineEdit::Normal,
                                       "", &userEnteredInput);

  if (userEnteredInput)
  {
    parentCourtroom->update_character_content_url(filesLink);
  }
}
