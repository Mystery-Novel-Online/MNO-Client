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
  m_Channels.clear();
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

void KeyframeSequence::SequenceJumpEnd()
{
  m_Timestamp = m_SequenceLength;
}

void KeyframeSequence::RunSequence(float deltaTime)
{
  if(m_SequenceLength == 0) return;
  if(m_Timestamp > m_SequenceLength && !m_Loop) return;
  if(m_Timestamp == 0) audio::effect::Play(m_SoundEffect.toStdString());
  m_Timestamp += deltaTime;
  while(m_Timestamp > m_SequenceLength)
  {
    if(!m_Loop) return;
    m_Timestamp -= m_SequenceLength;
  }
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
