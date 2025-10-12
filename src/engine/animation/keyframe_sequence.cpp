#include "keyframe_sequence.h"
#include "engine/system/animation.h"
#include "engine/system/audio.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/system/theme_scripting.h"

KeyframeSequence::KeyframeSequence()
{
  engine::system::animation::addAnimation(this);
}

KeyframeSequence::~KeyframeSequence()
{
  engine::system::animation::removeAnimation(this);
}

void KeyframeSequence::Cleanup()
{
  m_SoundEffect = "";
  m_Loop = false;
  m_SequenceLength = 0;
  m_Timestamp = 0;
  m_LastTimestamp = 0;
  m_Channels.clear();

  m_TimedSounds.clear();
  m_TriggeredThisLoop.clear();
  m_TimedSignals.clear();
  m_TriggeredSignalsThisLoop.clear();

  m_ViewportTimestamp = 0;
  m_RenderProcessed = false;
}

void KeyframeSequence::SetSound(QString name)
{
  m_SoundEffect = name;
}

void KeyframeSequence::SetLoop(bool isLoop)
{
  m_Loop = isLoop;
}

void KeyframeSequence::CalculateLength(const std::string &name)
{
  float channelLength = m_Channels[name]->GetLength();
  if(channelLength > m_SequenceLength) m_SequenceLength = channelLength;
}

void KeyframeSequence::AddChannel(const std::string &name, std::unique_ptr<KeyframeChannelTemplate> channel)
{
  m_Channels[name] = std::move(channel);
  float channelLength = m_Channels[name]->GetLength();
  if(channelLength > m_SequenceLength) m_SequenceLength = channelLength;
}

void KeyframeSequence::AddTimedSound(float timestamp, const std::string &sound)
{
  m_TimedSounds.push_back({timestamp, sound});
}

void KeyframeSequence::SequenceJumpEnd()
{
  m_Timestamp = m_SequenceLength;
}

void KeyframeSequence::RunSequence(float deltaTime)
{
  if(m_SequenceLength == 0) return;
  if(m_Timestamp > m_SequenceLength && !m_Loop)
  {
    m_Running = false;
    QString animationCall = "On" + m_FriendlyName + "End";
    if(m_FriendlyName.isEmpty())
      LuaBridge::LuaEventCall("OnAnimationEnd");
    else
      LuaBridge::LuaEventCall(animationCall.toStdString().c_str());
    return;
  }
  if(m_Timestamp == 0) audio::effect::Play(m_SoundEffect.toStdString());


  float newTimestamp = m_Timestamp + deltaTime;

  if (newTimestamp > m_SequenceLength && m_Loop)
  {
    newTimestamp -= m_SequenceLength;
    m_TriggeredThisLoop.clear();
  }

  for (const auto& soundEvent : m_TimedSounds) {
    float triggerTime = soundEvent.timestamp;
    bool wrapped = (newTimestamp < m_Timestamp);

    if ((wrapped && (triggerTime >= m_Timestamp || triggerTime <= newTimestamp)) ||
        (!wrapped && triggerTime >= m_Timestamp && triggerTime <= newTimestamp)) {
      if (m_TriggeredThisLoop.find(soundEvent.sound) == m_TriggeredThisLoop.end()) {
        audio::effect::Play(soundEvent.sound);
        m_TriggeredThisLoop.insert(soundEvent.sound);
      }
    }
  }

  for (const auto& signalEvent : m_TimedSignals) {
    float triggerTime = signalEvent.timestamp;
    bool wrapped = (newTimestamp < m_Timestamp);

    if ((wrapped && (triggerTime >= m_Timestamp || triggerTime <= newTimestamp)) ||
        (!wrapped && triggerTime >= m_Timestamp && triggerTime <= newTimestamp)) {
      if (m_TriggeredSignalsThisLoop.find(signalEvent.signal) == m_TriggeredSignalsThisLoop.end())
      {
        if(signalEvent.signal == "hide_desk")
          courtroom::layout::setVisibility("ui_vp_desk", false);
        else if(signalEvent.signal == "hide_textbox")
          courtroom::layout::setVisibility("ao2_chatbox", false);
        else if(signalEvent.signal == "show_textbox")
          courtroom::layout::setVisibility("ao2_chatbox", true);

        m_TriggeredSignalsThisLoop.insert(signalEvent.signal);
      }
    }
  }

  m_LastTimestamp = m_Timestamp;
  m_Timestamp = newTimestamp;
}

void KeyframeSequence::Evaluate(std::unordered_map<std::string, QVariant> &outValues) const
{
  for (const auto& pair : m_Channels)
  {
    QVariant value;
    pair.second->Evaluate(m_Timestamp, value);
    outValues[pair.first] = value;
  }
}

void KeyframeSequence::AddTimedSignal(float timestamp, const std::string &signal)
{
  m_TimedSignals.push_back({timestamp, signal});
}

bool KeyframeSequence::getLoopState()
{
  return m_Loop;
}

bool KeyframeSequence::canRenderViewport()
{
  if(m_RenderProcessed) return false;
  if(m_Timestamp >= m_ViewportTimestamp)
  {
    m_RenderProcessed = true;
  }
  return m_RenderProcessed;
}
