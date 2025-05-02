#include "emote_menu.h"

static bool s_sizeDoubled = false;
static bool s_renderSprites = false;

#include "aoapplication.h"
#include "courtroom.h"
#include "modules/managers/emotion_manager.h"
#include "modules/managers/character_manager.h"
#include "dro/interface/courtroom_layout.h"

EmoteMenu::EmoteMenu(QWidget *parent) : QMenu(parent)
{
  p_SizeAction = addAction(tr("Resize"));
  p_RenderAction = addAction(tr("Use Sprite Images"));
  addSeparator();

  m_presetsMenu = new QMenu(tr("Presets"), this);
  addMenu(m_presetsMenu);

  //p_makerAction = addAction(tr("Button Maker"));

  connect(p_SizeAction, &QAction::triggered, this, &EmoteMenu::OnDoubleSizeTriggered);
  connect(p_RenderAction, &QAction::triggered, this, &EmoteMenu::OnRealtimeTriggered);
  //connect(p_makerAction, &QAction::triggered, this, &EmoteMenu::OnButtonMakerTriggered);

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
  if(s_sizeDoubled)
  {
    s_sizeDoubled = false;
  }
  else
  {
    s_sizeDoubled = true;
  }

  AOApplication::getInstance()->get_courtroom()->construct_emote_page_layout();
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
  EmotionManager::get().refreshEmotePage(true);
}

void EmoteMenu::OnButtonMakerTriggered()
{
  m_buttonMaker->show();
  m_buttonMaker->SetCharacter(CharacterManager::get().p_SelectedCharacter->mFolder);
}

void EmoteMenu::ApplyPreset(const QString &presetName)
{

  for(ActorScalingPreset presetData : CharacterManager::get().p_SelectedCharacter->GetScalingPresets())
  {
    if(presetData.name == presetName)
    {
      Layout::Courtroom::SetScaleSlider(presetData.Scale);
      Layout::Courtroom::SetVerticalSlider(presetData.VerticalAlign);
    }
  }
  qDebug() << "Presets applied";
}
