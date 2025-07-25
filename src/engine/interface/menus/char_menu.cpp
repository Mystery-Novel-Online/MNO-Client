#include "char_menu.h"
#include "modules/theme/thememanager.h"
#include "engine/fs/fs_reading.h"
#include "engine/param/actor_repository.h"
#include "engine/param/actor/actor_loader.h"

using namespace engine::actor::user;

CharMenu::CharMenu(Courtroom *parent) : QMenu(parent), parentCourtroom(parent)
{
  p_UpdateCharacterFilesAction = addAction(tr("Set Character URL"));

  p_OpenDirectoryAction = addAction(tr("Open Character Directory"));

  p_RandomizeMenu = new QMenu(tr("Randomize Character"), this);
  addMenu(p_RandomizeMenu);



  p_RandomServerAction = p_RandomizeMenu->addAction("From Server");
  p_RandomFavoritesAction = p_RandomizeMenu->addAction("From Favorites");
  p_RandomFilesAction = p_RandomizeMenu->addAction("From Files");

  connect(p_UpdateCharacterFilesAction, &QAction::triggered, this, &CharMenu::OnUpdateCharacterFilesTriggered);
  connect(p_OpenDirectoryAction, &QAction::triggered, this, &CharMenu::OnOpenCharacterTriggered);
  connect(p_RandomServerAction, &QAction::triggered, this, &CharMenu::OnRandomizeServerTriggered);
  connect(p_RandomFavoritesAction, &QAction::triggered, this, &CharMenu::OnRandomizeFavoritesTriggered);
  connect(p_RandomFilesAction, &QAction::triggered, this, &CharMenu::OnRandomizeIniswapTriggered);
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

void CharMenu::OnOpenCharacterTriggered()
{
  QUrl folderUrl = QUrl::fromLocalFile(FS::Paths::FindDirectory("characters/" +  QString::fromStdString(retrieve()->folder())));
  QDesktopServices::openUrl(folderUrl);
}

void CharMenu::OnRandomizeServerTriggered()
{
  parentCourtroom->SwitchRandomCharacter("Server Characters");
}

void CharMenu::OnRandomizeFavoritesTriggered()
{
  parentCourtroom->SwitchRandomCharacter("Favorites");
}

void CharMenu::OnRandomizeIniswapTriggered()
{
  parentCourtroom->SwitchRandomCharacter("All");
}
