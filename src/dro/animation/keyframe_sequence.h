#ifndef KEYFRAME_SEQUENCE_H
#define KEYFRAME_SEQUENCE_H

#include <memory>
#include <string>
#include <unordered_set>
#include "dro/animation/keyframe_channel.h"

class KeyframeSequence
{
public:
  KeyframeSequence();
  ~KeyframeSequence();
  void Cleanup();
  void SetSound(QString name);
  void SetLoop(bool isLoop);
  void setViewportTimestamp(int time) { m_ViewportTimestamp = time; };


  void AddChannel(const std::string& name, std::unique_ptr<KeyframeChannelTemplate> channel);
  void AddTimedSound(float timestamp, const std::string& sound);
  void SequenceJumpEnd();
  void RunSequence(float deltaTime);
  void Evaluate(std::unordered_map<std::string, QVariant>& outValues) const;

  void AddTimedSignal(float timestamp, const std::string& signal);

  bool getLoopState();
  bool canRenderViewport();
  int viewportTimestamp() {return m_ViewportTimestamp;}

private:
  bool m_Loop = false;
  bool m_RenderProcessed = false;
  QString m_SoundEffect = "";

  float m_LastTimestamp = 0.0f;
  float m_Timestamp = 0.0f;

  int m_ViewportTimestamp = 0;


  float m_SequenceLength = 0.0f;
  std::unordered_map<std::string, std::unique_ptr<KeyframeChannelTemplate>> m_Channels;


  std::vector<TimedSound> m_TimedSounds;
  std::unordered_set<std::string> m_TriggeredThisLoop;


  std::vector<TimedSignal> m_TimedSignals;
  std::unordered_set<std::string> m_TriggeredSignalsThisLoop;


};

#endif // KEYFRAME_SEQUENCE_H
