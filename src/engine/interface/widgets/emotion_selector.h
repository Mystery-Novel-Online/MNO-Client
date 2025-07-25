#ifndef EMOTION_SELECTOR_H
#define EMOTION_SELECTOR_H

#include "engine/interface/menus/emote_menu.h"

#include <rolechat/actor/IActorData.h>

class AOEmoteButton;
class ActorData;

class EmotionSelector : public RPWidget
{
  Q_OBJECT
public:
  explicit EmotionSelector(QWidget *parent);

  void emotionChange(ActorEmote emote);
  void actorChange(rolechat::actor::IActorData *actor);
  void outfitChange();

  int calculateTrueIndex(int id);

  void constructEmotes();
  void refreshEmotes(bool scrollToCurrent);
  void refreshSelection(bool changedActor);
  void resetPage();

  ActorEmote getEmote(int emoteId);
  ActorEmote getSelectedEmote();
  int getSelectedIndex() { return m_SelectedIndex; }


  void switchPageNext();
  void switchPagePrevious();
  void selectEmote(int emoteTarget);

private:
  EmoteMenu* m_ContextMenu = nullptr;
  QVector<AOEmoteButton *> m_EmotionButtons;
  QVector<ActorEmote> m_ActorEmotions = {};

  int m_EmoteColumns = 5;
  int m_EmoteRows = 2;
  int m_PageLimit = 0;
  int m_PageIndex = 0;
  int m_SelectedIndex = 0;

public slots:
  void dropdownChanged(int id);
  void emoteClicked(int id);

private slots:
  void showEmoteTooltip(int id, QPoint pos);
  void hideEmoteTooltip(int id);

signals:

protected:
  void wheelEvent(QWheelEvent *event) override;

};

#endif // EMOTION_SELECTOR_H
