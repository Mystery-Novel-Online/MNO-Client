#ifndef ANIMATION_H
#define ANIMATION_H

class KeyframeSequence;

namespace dro::system::animation
{
  void runAll(float deltaTime);
  void removeAnimation(KeyframeSequence* sequence);
  void addAnimation(KeyframeSequence* sequence);
}
#endif // ANIMATION_H
