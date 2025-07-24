#ifndef ANIMATION_READER_H
#define ANIMATION_READER_H

#include "engine/param/json_reader.h"

class KeyframeSequence;

class AnimationReader : public JSONReader
{
public:
  AnimationReader(const QString &weatherPath, KeyframeSequence &sequence);
  AnimationReader(const QString &name, KeyframeSequence &sequence, const QString &character);
  AnimationReader(const QString &name, const QString &theme, KeyframeSequence &sequence);

  void loadData(KeyframeSequence &sequence);
  QVector<ActorLayer> m_Layers = {};
};

#endif // ANIMATION_READER_H
