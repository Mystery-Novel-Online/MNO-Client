#ifndef CHAR_MENU_H
#define CHAR_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>

class Courtroom;

class CharMenu : public QMenu
{
  Q_OBJECT
public:
  CharMenu(Courtroom *parent = nullptr);

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnUpdateCharacterFilesTriggered();
  void OnOpenCharacterTriggered();
  void OnRandomizeServerTriggered();
  void OnRandomizeFavoritesTriggered();
  void OnRandomizeIniswapTriggered();

private:
  QAction *p_UpdateCharacterFilesAction;
  QAction *p_OpenDirectoryAction;


  QMenu* p_RandomizeMenu;
  QAction *p_RandomServerAction;
  QAction *p_RandomFavoritesAction;
  QAction *p_RandomFilesAction;

  Courtroom *parentCourtroom;
};

#endif // CHAR_MENU_H
