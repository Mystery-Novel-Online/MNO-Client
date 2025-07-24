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
  void OnPinTriggered();
  void OnSmoothPlayAction();
  void OnInstantPlayAction();
  void OnSyncPlayAction();


private:
  QString m_TargetTrack = "";
  QAction *p_InsertAction = nullptr;
  QAction *p_PinAction = nullptr;

  QAction *m_PlaySmooth = nullptr;
  QAction *m_PlayInstant = nullptr;
  QAction *m_PlaySync = nullptr;

};

#endif // BGM_MENU_H
