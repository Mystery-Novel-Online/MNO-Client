#include "keyframe_sequence.h"
#include "dro/system/animation.h"

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
  m_SequenceLength = 0;
  m_Timestamp = 0;
  m_Channels.clear();
}

void KeyframeSequence::AddChannel(const std::string &name, std::unique_ptr<KeyframeChannelTemplate> channel)
{
  m_Channels[name] = std::move(channel);
  float channelLength = m_Channels[name]->GetLength();
  if(channelLength > m_SequenceLength) m_SequenceLength = channelLength;
}

void KeyframeSequence::RunSequence(float deltaTime)
{
  if(m_SequenceLength == 0) return;
  m_Timestamp += deltaTime;
  while(m_Timestamp > m_SequenceLength) m_Timestamp -= m_SequenceLength;
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
