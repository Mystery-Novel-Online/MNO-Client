#ifndef DIALOGUECUES_H
#define DIALOGUECUES_H

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
