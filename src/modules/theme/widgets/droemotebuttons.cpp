#include "droemotebuttons.h"
#include "courtroom.h"
#include "modules/managers/emotion_manager.h"
#include <QWheelEvent>
#include "dro/interface/menus/emote_menu.h"

DROEmoteButtons::DROEmoteButtons(QWidget *parent)
    : QWidget{parent}
{
  EmoteMenu *menu = new EmoteMenu(this);
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QWidget::customContextMenuRequested, menu, &EmoteMenu::OnMenuRequested);
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
