#ifndef ANIMATION_READER_H
#define ANIMATION_READER_H

#include "dro/param/json_reader.h"

class KeyframeSequence;

class AnimationReader : public JSONReader
{
public:
  AnimationReader(const QString &weatherPath, KeyframeSequence &sequence);
  AnimationReader(const QString &name, KeyframeSequence &sequence, const QString &character);

  void loadData(KeyframeSequence &sequence);
  QVector<EmoteLayer> m_Layers = {};
};

#endif // ANIMATION_READER_H
