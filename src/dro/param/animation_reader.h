#ifndef ANIMATION_READER_H
#define ANIMATION_READER_H

#include "dro/param/json_reader.h"

class KeyframeSequence;

class AnimationReader : public JSONReader
{
public:
  AnimationReader(QString name, KeyframeSequence &sequence);
};

#endif // ANIMATION_READER_H
