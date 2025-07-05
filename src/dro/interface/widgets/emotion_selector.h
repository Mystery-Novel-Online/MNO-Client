#ifndef EMOTION_SELECTOR_H
#define EMOTION_SELECTOR_H

#include <AOApplication.h>
#include <QObject>
#include <QWidget>
#include "dro/interface/widgets/rp_widget.h"
#include "dro/interface/menus/emote_menu.h"

class AOEmoteButton;

class EmotionSelector : public RPWidget
{
  Q_OBJECT
public:
  explicit EmotionSelector(QWidget *parent);

  void emotionChange(DREmote emote);
  void actorChange(ActorData *actor);
  void outfitChange();

  int calculateTrueIndex(int id);

  void constructEmotes();
  void refreshEmotes(bool scrollToCurrent);
  void refreshSelection(bool changedActor);
  void resetPage();

  DREmote getEmote(int emoteId);
  DREmote getSelectedEmote();
  int getSelectedIndex() { return m_SelectedIndex; }


  void switchPageNext();
  void switchPagePrevious();
  void selectEmote(int emoteTarget);

private:
  EmoteMenu* m_ContextMenu = nullptr;
  QVector<AOEmoteButton *> m_EmotionButtons;
  QVector<DREmote> m_ActorEmotions = {};

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
