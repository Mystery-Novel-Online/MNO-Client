#ifndef AREA_MENU_H
#define AREA_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>

class AreaMenu : public QMenu
{
  Q_OBJECT
public:
  AreaMenu(QWidget *parent = nullptr);

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnLockTriggered();
  void OnPeekTriggered();

private:
  int m_TargetArea = -1;
  QAction *p_LockAction = nullptr;
  QAction *p_PeekAction = nullptr;
};

#endif // AREA_MENU_H
