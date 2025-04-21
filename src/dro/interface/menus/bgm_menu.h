#ifndef BGM_MENU_H
#define BGM_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>

class BGMMenu : public QMenu
{
  Q_OBJECT
public:
  BGMMenu(QWidget *parent = nullptr);

public slots:
  void OnMenuRequested(QPoint p_point);
  void OnInsertTriggered();
  void OnPlayTriggered();
  void OnPinTriggered();

private:
  QString m_TargetTrack = "";
  QAction *p_InsertAction = nullptr;
  QAction *p_PlayAction = nullptr;
  QAction *p_PinAction = nullptr;

};

#endif // BGM_MENU_H
