#include "pair_manager.h"
#include "commondefs.h"

#include <AOApplication.h>

#include <modules/theme/thememanager.h>
#include "dro/interface/courtroom_layout.h"

PairManager PairManager::s_Instance;

bool PairManager::GetUsePairData()
{
  return m_pairActive;
}

void PairManager::SetPairData(QString character, QString emote, int selfOffset, int pairOffset, bool flipped, int scale, int vertical)
{
  int l_CourtroomWidth = ThemeManager::get().getWidget("viewport")->width();

  if (l_CourtroomWidth != 0) {
    mSelfOffset = (selfOffset * l_CourtroomWidth) / 960;
  } else {
    mSelfOffset = selfOffset;
  }

  if (l_CourtroomWidth != 0) {
    mPairOffset = (pairOffset * l_CourtroomWidth) / 960;
  } else {
    mPairOffset = pairOffset;
  }

  mCharacter = character;
  mEmote = emote;
  m_pairActive = true;
  m_pairFlipped = flipped;
  if(scale != 0) mScaleOffset = scale;
  else mScaleOffset = 1000;
  mVerticalOffset = vertical;
}

int PairManager::GetOffsetSelf()
{
  int courtroomWidth = ThemeManager::get().getWidget("viewport")->width();
  int halfWidth = courtroomWidth / 2;
  return mSelfOffset - halfWidth;
}

int PairManager::GetOffsetOther()
{
  int courtroomWidth = ThemeManager::get().getWidget("viewport")->width();
  int halfWidth = courtroomWidth / 2;
  return mPairOffset - halfWidth;
}

QString PairManager::GetEmoteName()
{
  return mEmote;
}

QString PairManager::GetCharacterFolder()
{
  return mCharacter;
}

bool PairManager::GetCharacterFlipped()
{
  return m_pairFlipped;
}

void PairManager::DisableUpcomingPair()
{
  m_pairActive = false;
  double l_CourtroomWidth = static_cast<double>(ThemeManager::get().getWidget("viewport")->width());
  mPairOffset = static_cast<int>(l_CourtroomWidth / 2);
  mSelfOffset = mPairOffset;
}

void PairManager::SetUserPair(int partner, int offset)
{
  mLocalPair = partner;
  mLocalOffset = offset;
  if(partner == -1)
  {
    courtroom::sliders::setHorizontal(480);
  }
  else
  {
    courtroom::sliders::setHorizontal(mLocalOffset);
  }
  //if(partner != -1) pSliderWidget->show();
  //else{ pSliderWidget->hide();}
}

bool PairManager::GetCanPair()
{
  return mLocalPair == -1;
}

bool PairManager::GetSpriteIsVisible()
{
  if(mEmote == "../../misc/blank") return false;
  if(mEmote.isEmpty()) return false;

  return m_pairActive;
}

void PairManager::ThemeReload()
{
  m_chatboxOffsetTransforms = {};
  QStringList shownameElements = {"showname", "ao2_chatbox", "message", "chat_arrow"};
  QStringList alignmentElements = {"", "_left", "_right"};

  for(QString elementname : shownameElements)
  {
    for(QString elementalighn : alignmentElements)
    {
      QString fullName = elementname + elementalighn;
      pos_size_type sizing = AOApplication::getInstance()->get_element_dimensions(fullName, COURTROOM_DESIGN_INI);
      if(sizing.height != 0 && sizing.width != 0)
      {
        m_chatboxOffsetTransforms[fullName] = sizing;
      }
    }
  }

}

pos_size_type PairManager::GetElementAlignment(QString name, QString alighment)
{
  QString alignmentName = name + "_" + alighment;

  if(m_chatboxOffsetTransforms.contains(alignmentName)) return m_chatboxOffsetTransforms[alignmentName];
  else if(m_chatboxOffsetTransforms.contains(name)) return m_chatboxOffsetTransforms[name];

  pos_size_type return_value;
  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;

  return return_value;

}
