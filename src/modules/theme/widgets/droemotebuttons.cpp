#include "droemotebuttons.h"
#include "courtroom.h"
#include "modules/managers/emotion_manager.h"
#include <QWheelEvent>

DROEmoteButtons::DROEmoteButtons(QWidget *parent)
    : QWidget{parent}
{
  m_ContextMenu = new EmoteMenu(this);
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QWidget::customContextMenuRequested, m_ContextMenu, &EmoteMenu::OnMenuRequested);
}

void DROEmoteButtons::EmoteChange(DREmote emote)
{
  if(m_ContextMenu == nullptr) return;
  m_ContextMenu->EmoteChange(emote);
}

void DROEmoteButtons::ActorChange(ActorData *actor)
{
  m_ContextMenu->ClearPresets();
  for(ActorScalingPreset presetData : actor->GetScalingPresets())
  {
    m_ContextMenu->AddPreset(presetData.name);
  }
}

void DROEmoteButtons::wheelEvent(QWheelEvent *event)
{
  int delta = event->angleDelta().y();

  if (delta > 0)
  {
    EmotionManager::get().execEmotePagePrevious();
  }
  else if (delta < 0)
  {
    EmotionManager::get().execEmotePageNext();
  }

  QWidget::wheelEvent(event);

}
