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

private:
  QAction *p_UpdateCharacterFilesAction;
  Courtroom *parentCourtroom;
};

#endif // CHAR_MENU_H
