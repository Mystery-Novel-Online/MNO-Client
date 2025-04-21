#include "emotion_manager.h"
#include "../theme/thememanager.h"
#include "character_manager.h"

#include <QtMath>
#include <QCheckBox>
#include "scene_manager.h"
#include <modules/theme/widgets/droemotebuttons.h>

EmotionManager EmotionManager::s_Instance;

QVector<DREmote> EmotionManager::getEmoteList()
{
  return mEmoteList;
}

DREmote EmotionManager::getCurrentEmote()
{
  return getEmote(mEmoteID);
}


void EmotionManager::setEmotesPanel(DROEmoteButtons *t_emotePanel)
{
  wEmotesPanel = t_emotePanel;
}

DREmote EmotionManager::getEmote(const int t_emoteID)
{
  if (t_emoteID < 0 || t_emoteID >= mEmoteList.length())
    return DREmote();
  return mEmoteList.at(t_emoteID);
}

void EmotionManager::fillEmoteDropdown()
{
  QWidget *w_emoteDropdown = ThemeManager::get().getWidget("emote_dropdown");

  if (dynamic_cast<QComboBox*>(w_emoteDropdown) != nullptr)
  {
    QComboBox* l_emoteCombobox = dynamic_cast<QComboBox*>(w_emoteDropdown);
    QSignalBlocker l_blocker(l_emoteCombobox);
    l_emoteCombobox->clear();

    QStringList l_emote_list;
    for (const DREmote &i_emote : qAsConst(mEmoteList))
      l_emote_list.append(i_emote.comment);
    l_emoteCombobox->addItems(l_emote_list);
  }
}

void EmotionManager::refreshEmoteSelection(bool t_switchedCharacter)
{
  const int l_prev_emote_count = mEmoteList.count();
  mEmoteList = CharacterManager::get().p_SelectedCharacter->getEmotes();
  
  QComboBox* l_emoteCombobox = dynamic_cast<QComboBox*>(ThemeManager::get().getWidget("emote_dropdown"));
  QCheckBox* l_preCheckbox = dynamic_cast<QCheckBox*>(ThemeManager::get().getWidget("pre"));

  const QString l_prev_emote = l_emoteCombobox->currentText();

  fillEmoteDropdown();

  if (t_switchedCharacter || l_prev_emote_count != mEmoteList.count())
  {
    mEmoteID = 0;
    mCurrentEmotePage = 0;
    l_preCheckbox->setChecked(l_preCheckbox->isChecked() || SceneManager::get().pConfigAO->always_pre_enabled());
  }
  else
  {
    l_emoteCombobox->setCurrentText(l_prev_emote);
  }
}

void EmotionManager::resetEmotePage()
{
  mEmoteID = 0;
  mCurrentEmotePage = 0;
  
  QWidget *w_emoteDropdown = ThemeManager::get().getWidget("emote_dropdown");

  if (dynamic_cast<QComboBox*>(w_emoteDropdown) != nullptr)
  {
    QComboBox* l_emoteCombobox = dynamic_cast<QComboBox*>(w_emoteDropdown);
    if (l_emoteCombobox->count())
      l_emoteCombobox->setCurrentIndex(mEmoteID);
  }

  refreshEmotePage(true);
}

void EmotionManager::refreshEmotePage(const bool t_scrollToCurrentEmote)
{

  AOButton* l_emotesLeft = ThemeManager::get().GetButton("emote_left");
  AOButton* l_emotesRight = ThemeManager::get().GetButton("emote_right");

  l_emotesLeft->hide();
  l_emotesRight->hide();

  for (AOEmoteButton *i_button : qAsConst(wEmoteList))
    i_button->hide();

  //AOApplication::getInstance()->m_courtroom->hide_emote_tooltip(mEmotePreviewID);

  //if (AOApplication::getInstance()->m_courtroom->is_spectating())
  //  return;

  const int l_emote_count = mEmoteList.length();
  const int l_page_count = qFloor(l_emote_count / mEmotePageMax) + bool(l_emote_count % mEmotePageMax);

  if (t_scrollToCurrentEmote)
    mCurrentEmotePage = mEmoteID / mEmotePageMax;
  mCurrentEmotePage = qBound(0, mCurrentEmotePage, l_page_count - 1);

  const int l_current_page_emote_count = qBound(0, l_emote_count - mCurrentEmotePage * mEmotePageMax, mEmotePageMax);

  if (mCurrentEmotePage + 1 < l_page_count)
    l_emotesRight->show();

  if (mCurrentEmotePage > 0)
    l_emotesLeft->show();

  for (int i = 0; i < l_current_page_emote_count; ++i)
  {
    const int l_real_i = i + mCurrentEmotePage * mEmotePageMax;
    AOEmoteButton *l_button = wEmoteList.at(i);
    l_button->set_image(getEmote(l_real_i), l_real_i == mEmoteID);
    l_button->show();
  }
}

void EmotionManager::constructEmotes()
{

}

void EmotionManager::constructEmotePageLayout()
{

}

void EmotionManager::execEmotePageNext()
{
  ++mCurrentEmotePage;
  refreshEmotePage();
}

void EmotionManager::execEmotePagePrevious()
{
  --mCurrentEmotePage;
  refreshEmotePage();
}

void EmotionManager::execSelectEmote(int t_id)
{
  const int l_min = mCurrentEmotePage * mEmotePageMax;
  const int l_max = (mEmotePageMax - 1) + mCurrentEmotePage * mEmotePageMax;

  const DREmote &l_prev_emote = getCurrentEmote();
  if (mEmoteID >= l_min && mEmoteID <= l_max)
  {
    AOEmoteButton *l_prev_button = wEmoteList.at(mEmoteID % mEmotePageMax);
    l_prev_button->set_image(l_prev_emote, false);
    l_prev_button->repaint();
  }

  const int l_prev_emote_id = mEmoteID;
  mEmoteID = t_id;
  const DREmote &l_emote = getCurrentEmote();

  if (mEmoteID >= l_min && mEmoteID <= l_max)
  {
    AOEmoteButton *l_new_button = wEmoteList.at(mEmoteID % mEmotePageMax);
    l_new_button->set_image(l_emote, true);
    l_new_button->repaint();
  }

  const int emote_mod = l_emote.modifier;
  
  QCheckBox* l_preCheckbox = dynamic_cast<QCheckBox*>(ThemeManager::get().getWidget("pre"));
  if (l_prev_emote_id == mEmoteID) // toggle
    l_preCheckbox->setChecked(!l_preCheckbox->isChecked());
  else
    l_preCheckbox->setChecked(emote_mod == 1 || SceneManager::get().pConfigAO->always_pre_enabled());


}

