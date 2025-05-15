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

  p_ResetOffsetsAction = addAction(tr("Reset Offsets"));
  m_presetsMenu = new QMenu(tr("Presets"), this);
  addMenu(m_presetsMenu);

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
}

void EmoteMenu::AddPreset(const QString &name)
{
  QAction* action = m_presetsMenu->addAction(name);
  connect(action, &QAction::triggered, this, [=]() { ApplyPreset(name); });
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
  courtroom::sliders::setScale(1000);
  courtroom::sliders::setVertical(0);
  courtroom::sliders::setHorizontal(480);
}

void EmoteMenu::ApplyPreset(const QString &presetName)
{
  for(ActorScalingPreset presetData : CharacterManager::get().p_SelectedCharacter->GetScalingPresets())
  {
    if(presetData.name == presetName)
    {
      courtroom::sliders::setScale(presetData.scale);
      courtroom::sliders::setVertical(presetData.verticalAlign);
    }
  }
}
