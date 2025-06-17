#ifndef KEYFRAME_SEQUENCE_H
#define KEYFRAME_SEQUENCE_H

#include <memory>
#include <string>
#include "dro/animation/keyframe_channel.h"

class KeyframeSequence
{
public:
  KeyframeSequence();
  ~KeyframeSequence();
  void Cleanup();
  void SetSound(QString name);
  void SetLoop(bool isLoop);
  void AddChannel(const std::string& name, std::unique_ptr<KeyframeChannelTemplate> channel);

  void SequenceJumpEnd();
  void RunSequence(float deltaTime);
  void Evaluate(std::unordered_map<std::string, QVariant>& outValues) const;

  bool getLoopState();

private:
  bool m_Loop = false;
  QString m_SoundEffect = "";
  float m_Timestamp = 0.0f;
  float m_SequenceLength = 0.0f;
  std::unordered_map<std::string, std::unique_ptr<KeyframeChannelTemplate>> m_Channels;
};

#endif // KEYFRAME_SEQUENCE_H
