#ifndef KEYFRAME_SEQUENCE_H
#define KEYFRAME_SEQUENCE_H

#include <memory>
#include <string>
#include <unordered_set>
#include "engine/animation/keyframe_channel.h"

class KeyframeSequence
{
public:
  KeyframeSequence();
  ~KeyframeSequence();
  void Cleanup();
  void SetSound(QString name);
  void SetLoop(bool isLoop);
  void setViewportTimestamp(int time) { m_ViewportTimestamp = time; };


  void CalculateLength(const std::string& name);
  void AddChannel(const std::string& name, std::unique_ptr<KeyframeChannelTemplate> channel);

  template<typename T>
  KeyframeChannel<T>* GetChannel(const std::string &name)
  {
    auto it = m_Channels.find(name);
    if (it != m_Channels.end())
      return dynamic_cast<KeyframeChannel<T>*>(it->second.get());
    return nullptr;
  }

  void AddTimedSound(float timestamp, const std::string& sound);
  void SequenceJumpEnd();
  virtual void RunSequence(float deltaTime);
  void Evaluate(std::unordered_map<std::string, QVariant>& outValues) const;

  void AddTimedSignal(float timestamp, const std::string& signal);

  bool getLoopState();
  bool canRenderViewport();
  int viewportTimestamp() {return m_ViewportTimestamp;}

  bool runningState() {return m_Running;}
  void setRunningState(bool state) {m_Running = state;}
  QString friendlyName() {return m_FriendlyName;}
  void setFriendlyName(QString name) {m_FriendlyName = name;}

private:
  QString m_FriendlyName = "";
  bool m_Running = true;
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
