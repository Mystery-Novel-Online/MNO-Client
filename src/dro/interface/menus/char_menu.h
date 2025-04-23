#ifndef CHAR_MENU_H
#define CHAR_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>

class CharMenu : public QMenu
{
  Q_OBJECT
public:
  CharMenu(QWidget *parent = nullptr);

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnUpdateCharacterFilesTriggered();

private:
  QAction *p_UpdateCharacterFilesAction;
};

#endif // CHAR_MENU_H
