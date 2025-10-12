#include "animation.h"
#include "engine/animation/keyframe_sequence.h"

QVector<KeyframeSequence*> s_currentSequences = {};

namespace engine::system::animation
{
  void runAll(float deltaTime)
  {
    for(auto sequence : s_currentSequences)
    {
      if(sequence->runningState())
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
