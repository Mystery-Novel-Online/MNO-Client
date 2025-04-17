#include "emote_menu.h"

static bool s_sizeDoubled = false;
static bool s_renderSprites = false;

#include "aoapplication.h"
#include "courtroom.h"
#include "modules/managers/emotion_manager.h"

EmoteMenu::EmoteMenu(QWidget *parent) : QMenu(parent)
{
  p_SizeAction = addAction(tr("Resize"));
  p_RenderAction = addAction(tr("Use Sprite Images"));

  connect(p_SizeAction, &QAction::triggered, this, &EmoteMenu::OnDoubleSizeTriggered);
  connect(p_RenderAction, &QAction::triggered, this, &EmoteMenu::OnRealtimeTriggered);\
}

bool EmoteMenu::isRealtime()
{
  return s_renderSprites;
}

bool EmoteMenu::isDoubleSize()
{
  return s_sizeDoubled;
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
