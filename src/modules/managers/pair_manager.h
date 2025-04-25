#ifndef PAIRMANAGER_H
#define PAIRMANAGER_H

#include "datatypes.h"
#include <QSlider>
#include <QString>



class PairManager
{
public:
  PairManager(const PairManager&) = delete;

  static PairManager& get()
  {
    return s_Instance;
  }

  void SetSlider(QSlider *slider);

  bool GetUsePairData();
  void SetPairData(QString character, QString emote, int selfOffset, int pairOffset, bool flipped, int scale, int vertical);
  double GetOffsetSelf();
  double GetOffsetOther();
  double GetVerticalOffset() {return (double)mVerticalOffset / (double)1000.0f; }
  double GetScaleoffset() {return (double)mScaleOffset / (double)1000.0f; }

  QString GetEmoteName();
  QString GetCharacterFolder();
  bool GetCharacterFlipped();

  void DisableUpcomingPair();

  void SetUserPair(int partner, int offset);
  bool GetCanPair();
  bool GetSpriteIsVisible();


  void ThemeReload();

  pos_size_type GetElementAlignment(QString name, QString alighment);
private:
  PairManager() {}
  static PairManager s_Instance;

  bool mPairUsed = true;
  bool mIsFlipped = false;

  QString mCharacter = "DUMMY";
  QString mEmote = "DUMMY";
  int mSelfOffset = 0;
  int mPairOffset = 0;
  int mScaleOffset = 1000;
  int mVerticalOffset = 0;

  int mLocalPair = -1;
  int mLocalOffset = 0;

  QSlider *pSliderWidget = nullptr;

  QHash<QString, pos_size_type> mPairChatboxPositions = {};
};

#endif // PAIRMANAGER_H
