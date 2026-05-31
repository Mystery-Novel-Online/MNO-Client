#ifndef DIALOGUECUES_H
#define DIALOGUECUES_H

inline constexpr int CueSpeedMin = 1;
inline constexpr int CueSpeedMax = 150;
inline constexpr int CueWaitMax = 3000;

enum class CueType
{
  SoundEffect,
  MusicChange,
  EmoteChange,
  PlaySequence,
  Wait,
  Speed,
  Effect,
  Flip,
  Hide,
  NewLine,
  Color,
  Size,
  Layer,
  SizeEnd,
  ColorEnd,
  Blip,
};

struct CueData
{
  int timestamp = 0;
  CueType type;
  QVariantList arguments;
};


#endif // DIALOGUECUES_H
