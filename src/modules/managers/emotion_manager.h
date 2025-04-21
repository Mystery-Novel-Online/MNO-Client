#ifndef EMOTIONMANAGER_H
#define EMOTIONMANAGER_H

#include <AOEmoteButton.h>
#include "modules/theme/widgets/droemotebuttons.h"



class EmotionManager
{
public:
  EmotionManager(const EmotionManager&) = delete;

  static EmotionManager& get()
  {
    return s_Instance;
  }

  void setEmotesPanel(DROEmoteButtons* t_emotePanel);
  DREmote getEmote(const int t_emoteID);
  DREmote getCurrentEmote();
  QVector<DREmote> getEmoteList();

  void resetEmotePage();

  void refreshEmoteSelection(bool t_switchedCharacter);
  void refreshEmotePage(const bool t_scrollToCurrentEmote = false);

  void constructEmotes();
  void constructEmotePageLayout();

  void fillEmoteDropdown();

  void execEmotePageNext();
  void execEmotePagePrevious();
  void execSelectEmote(int t_id);


  QVector<DREmote> mEmoteList = {};
  QVector<AOEmoteButton *> wEmoteList;

  int mEmoteID = 0;
  int mCurrentEmotePage = 0;
  int mEmotePreviewID = -1;
  int mEmotePageMax = 10;

  QWidget *wEmotesPanel = nullptr;

private:
  EmotionManager() {}
  static EmotionManager s_Instance;

};

#endif // EMOTIONMANAGER_H
