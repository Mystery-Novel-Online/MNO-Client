#ifndef EMOTE_MENU_H
#define EMOTE_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>


class EmoteMenu : public QMenu
{
  Q_OBJECT
public:
  EmoteMenu(QWidget *parent = nullptr);

  static bool isRealtime();
  static bool isDoubleSize();
public slots:
  void OnMenuRequested(QPoint p_point);
  void OnDoubleSizeTriggered();
  void OnRealtimeTriggered();

private:
  bool m_DoubleSize = false;
  bool m_Realtime = false;
  QAction *p_SizeAction = nullptr;
  QAction *p_RenderAction = nullptr;

};

#endif // EMOTE_MENU_H
