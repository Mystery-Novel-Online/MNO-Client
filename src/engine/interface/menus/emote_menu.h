#ifndef EMOTE_MENU_H
#define EMOTE_MENU_H

#include <QMenu>
#include <QObject>
#include <QWidget>
#include "engine/tools/button_maker.h"

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

  void clearLayers();
  void AddLayer(const QString& name, bool defaultValue);


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
  QMenu* m_layersMenu;


  DREmote m_currentEmote;
  //
  bool m_presetsClearedCheck = false;
  int m_defaultVertical = 0;
  int m_defaultScale = 1000;
};

#endif // EMOTE_MENU_H
