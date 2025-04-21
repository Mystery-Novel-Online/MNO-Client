#ifndef EMOTE_MENU_H
#define EMOTE_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>
#include "dro/tools/button_maker.h"


class EmoteMenu : public QMenu
{
  Q_OBJECT
public:
  EmoteMenu(QWidget *parent = nullptr);
  void EmoteChange(DREmote emote);

  static bool isRealtime();
  static bool isDoubleSize();


public slots:
  void OnMenuRequested(QPoint p_point);
  void OnDoubleSizeTriggered();
  void OnRealtimeTriggered();
  void OnButtonMakerTriggered();

private:
  ButtonMaker *m_buttonMaker = nullptr;
  bool m_DoubleSize = false;
  bool m_Realtime = false;
  QAction *p_SizeAction = nullptr;
  QAction *p_RenderAction = nullptr;
  QAction *p_makerAction = nullptr;

};

#endif // EMOTE_MENU_H
