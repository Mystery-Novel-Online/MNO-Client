#include "keyframe_sequence.h"
#include "dro/system/animation.h"
#include "dro/system/audio.h"

KeyframeSequence::KeyframeSequence()
{
  dro::system::animation::addAnimation(this);
}

KeyframeSequence::~KeyframeSequence()
{
  dro::system::animation::removeAnimation(this);
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
}

void KeyframeSequence::SetSound(QString name)
{
  m_SoundEffect = name;
}

void KeyframeSequence::SetLoop(bool isLoop)
{
  m_Loop = isLoop;
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
  if(m_Timestamp > m_SequenceLength && !m_Loop) return;
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

bool KeyframeSequence::getLoopState()
{
  return m_Loop;
}
