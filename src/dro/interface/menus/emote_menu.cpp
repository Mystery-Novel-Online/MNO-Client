#include "emote_menu.h"

static bool s_sizeDoubled = false;
static bool s_renderSprites = false;

#include "aoapplication.h"
#include "courtroom.h"
#include "modules/managers/character_manager.h"
#include "dro/interface/courtroom_layout.h"

EmoteMenu::EmoteMenu(EmotionSelector *parent) : QMenu(parent)
{
  m_EmotionSelector = parent;
  p_SizeAction = addAction(tr("Resize"));
  p_RenderAction = addAction(tr("Use Sprite Images"));
  addSeparator();


  m_presetsMenu = new QMenu(tr("Offsets"), this);
  p_ResetOffsetsAction = new QAction(tr("Reset (Center)"), this);
  addMenu(m_presetsMenu);
  m_presetsMenu->addAction(p_ResetOffsetsAction);

  m_layersMenu = new QMenu(tr("Toggles"), this);
  addMenu(m_layersMenu);

  p_makerAction = addAction(tr("Button Maker"));

  connect(p_SizeAction, &QAction::triggered, this, &EmoteMenu::OnDoubleSizeTriggered);
  connect(p_RenderAction, &QAction::triggered, this, &EmoteMenu::OnRealtimeTriggered);
  connect(p_ResetOffsetsAction, &QAction::triggered, this, &EmoteMenu::OnOffsetResetTriggered);
  connect(p_makerAction, &QAction::triggered, this, &EmoteMenu::OnButtonMakerTriggered);

  m_buttonMaker = new ButtonMaker();
  m_buttonMaker->resize(960, 544);
  m_buttonMaker->hide();
}

void EmoteMenu::EmoteChange(DREmote emote)
{
  if(m_buttonMaker == nullptr) return;
  m_buttonMaker->SetEmote(emote);
}

bool EmoteMenu::isRealtime()
{
  return s_renderSprites;
}

bool EmoteMenu::isDoubleSize()
{
  return s_sizeDoubled;
}

void EmoteMenu::ClearPresets()
{
  m_presetsMenu->clear();
  m_presetsMenu->addAction(p_ResetOffsetsAction);
  m_defaultVertical = 0;
  m_defaultScale = 1000;
  m_presetsClearedCheck = false;
}

void EmoteMenu::AddPreset(const QString &name)
{
  QAction* action = m_presetsMenu->addAction(name);
  connect(action, &QAction::triggered, this, [=]() { ApplyPreset(name); });

  if(m_presetsClearedCheck) return;

  for(ActorScalingPreset presetData : CharacterManager::get().p_SelectedCharacter->GetScalingPresets())
  {
    if(presetData.name == name)
    {
      m_defaultVertical = presetData.verticalAlign;
      m_defaultScale = presetData.scale;
    }
  }
  m_presetsClearedCheck = true;
}

void EmoteMenu::OnMenuRequested(QPoint p_point)
{
  const QPoint l_global_point = parentWidget()->mapToGlobal(p_point);
  popup(l_global_point);
}

void EmoteMenu::OnDoubleSizeTriggered()
{
  s_sizeDoubled = s_sizeDoubled == false;
  m_EmotionSelector->constructEmotes();
}

void EmoteMenu::OnRealtimeTriggered()
{
  if(s_renderSprites)
  {
    p_RenderAction->setText("Use Sprite Images");
    s_renderSprites = false;
  }
  else
  {
    p_RenderAction->setText("Use Button Images");
    s_renderSprites = true;
  }
  m_EmotionSelector->refreshEmotes(true);
}

void EmoteMenu::OnButtonMakerTriggered()
{
  m_buttonMaker->show();
  m_buttonMaker->SetCharacter(CharacterManager::get().p_SelectedCharacter->GetFolder());
}

void EmoteMenu::OnOffsetResetTriggered()
{
  courtroom::sliders::setScale(m_defaultScale);
  courtroom::sliders::setVertical(m_defaultVertical);
  courtroom::sliders::setHorizontal(500);
}

void EmoteMenu::ApplyPreset(const QString &presetName)
{
  for(ActorScalingPreset presetData : CharacterManager::get().p_SelectedCharacter->GetScalingPresets())
  {
    if(presetData.name == presetName)
    {
      courtroom::sliders::setScale(presetData.scale);
      courtroom::sliders::setVertical(presetData.verticalAlign);
      m_defaultVertical = presetData.verticalAlign;
      m_defaultScale = presetData.scale;
    }
  }
}
