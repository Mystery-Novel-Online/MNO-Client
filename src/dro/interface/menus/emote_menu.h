#ifndef EMOTE_MENU_H
#define EMOTE_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>
#include "dro/tools/button_maker.h"

class EmotionSelector;

class EmoteMenu : public QMenu
{
  Q_OBJECT
public:
  EmoteMenu(EmotionSelector *parent = nullptr);
  void EmoteChange(DREmote emote);

  static bool isRealtime();
  static bool isDoubleSize();

  void ClearPresets();
  void AddPreset(const QString& name);


public slots:
  void OnMenuRequested(QPoint p_point);
  void OnDoubleSizeTriggered();
  void OnRealtimeTriggered();
  void OnButtonMakerTriggered();
  void OnOffsetResetTriggered();
  void ApplyPreset(const QString& presetName);

private:
  EmotionSelector *m_EmotionSelector = nullptr;
  ButtonMaker *m_buttonMaker = nullptr;
  bool m_DoubleSize = false;
  bool m_Realtime = false;

  QAction *p_SizeAction = nullptr;
  QAction *p_RenderAction = nullptr;
  QAction *p_makerAction = nullptr;

  QAction *p_ResetOffsetsAction = nullptr;

  QMenu* m_presetsMenu;

};

#endif // EMOTE_MENU_H
