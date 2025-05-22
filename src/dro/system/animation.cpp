#include "animation.h"
#include "dro/animation/keyframe_sequence.h"
#include <QVector>

QVector<KeyframeSequence*> s_currentSequences = {};

namespace dro::system::animation
{
  void runAll(float deltaTime)
  {
    for(auto sequence : s_currentSequences)
    {
      sequence->RunSequence(deltaTime);
    }
  }

  void addAnimation(KeyframeSequence *sequence)
  {
    s_currentSequences.append(sequence);
  }

  void removeAnimation(KeyframeSequence *sequence)
  {
    if(s_currentSequences.contains(sequence)) s_currentSequences.removeAll(sequence);
  }

}
