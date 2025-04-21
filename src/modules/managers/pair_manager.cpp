#include "pair_manager.h"
#include "commondefs.h"

#include <AOApplication.h>

#include <modules/theme/thememanager.h>

PairManager PairManager::s_Instance;

void PairManager::SetSlider(QSlider *slider)
{
  pSliderWidget = slider;
}

bool PairManager::GetUsePairData()
{
  return mPairUsed;
}

void PairManager::SetPairData(QString character, QString emote, int selfOffset, int pairOffset, bool flipped)
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
  mPairUsed = true;
  mIsFlipped = flipped;
}

double PairManager::GetOffsetSelf()
{
  double l_CourtroomWidth = static_cast<double>(ThemeManager::get().getWidget("viewport")->width());
  double l_HalfCourtroomWidth = static_cast<double>(l_CourtroomWidth / 2);

  return static_cast<double>(((double)mSelfOffset / l_CourtroomWidth) * l_CourtroomWidth - l_HalfCourtroomWidth);

  //return ((double)mSelfOffset / 960) * 960 - 480;
}

double PairManager::GetOffsetOther()
{
  double l_CourtroomWidth = static_cast<double>(ThemeManager::get().getWidget("viewport")->width());
  double l_HalfCourtroomWidth = static_cast<double>(l_CourtroomWidth / 2);

  return static_cast<double>(((double)mPairOffset / l_CourtroomWidth) * l_CourtroomWidth - l_HalfCourtroomWidth);
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
  return mIsFlipped;
}

void PairManager::DisableUpcomingPair()
{
  mPairUsed = false;
  double l_CourtroomWidth = static_cast<double>(ThemeManager::get().getWidget("viewport")->width());
  mPairOffset = static_cast<int>(l_CourtroomWidth / 2);
  mSelfOffset = mPairOffset;
}

void PairManager::SetUserPair(int partner, int offset)
{
  mLocalPair = partner;
  mLocalOffset = offset;
  pSliderWidget->setValue(mLocalOffset);
  if(partner != -1) pSliderWidget->show();
  else{ pSliderWidget->hide();}
}

bool PairManager::GetCanPair()
{
  return mLocalPair == -1;
}

bool PairManager::GetSpriteIsVisible()
{
  if(mEmote == "../../misc/blank") return false;
  if(mEmote.isEmpty()) return false;

  return mPairUsed;
}

void PairManager::ThemeReload()
{
  mPairChatboxPositions = {};
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
        mPairChatboxPositions[fullName] = sizing;
      }
    }
  }

}

pos_size_type PairManager::GetElementAlignment(QString name, QString alighment)
{
  QString alignmentName = name + "_" + alighment;

  if(mPairChatboxPositions.contains(alignmentName)) return mPairChatboxPositions[alignmentName];
  else if(mPairChatboxPositions.contains(name)) return mPairChatboxPositions[name];

  pos_size_type return_value;
  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;

  return return_value;

}
