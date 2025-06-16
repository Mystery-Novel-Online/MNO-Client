#include "aoapplication.h"

#include "draudiostream.h"
#include <QDebug>

#include "ambience_player.h"

RPAmbiencePlayer::RPAmbiencePlayer(QObject *p_parent)
    : AOObject(p_parent)
{
  m_family = DRAudioEngine::get_family(DRAudio::Family::FEffect);
}

void RPAmbiencePlayer::play(QString ambienceName)
{
  if(ambienceName == m_filename) return;
  m_filename = ambienceName;

  QSharedPointer<DRAudioStream> newAmbience = m_family->create_stream(ambienceName);

  if(!newAmbience)
  {
    if (m_currentAmbience)
      m_currentAmbience->fadeOut(600);
    return;
  }

  newAmbience->set_repeatable(true);

  if (m_currentAmbience)
    m_currentAmbience->fadeOut(600);
  newAmbience->fadeIn(600);
  newAmbience->play();

  newAmbience->set_speed(0.0f);
  newAmbience->toggle_reverb(false);

  m_lastAmbience = m_currentAmbience;
  m_currentAmbience = newAmbience;
}

void RPAmbiencePlayer::toggleReverb(bool reverb)
{
  if(m_currentAmbience == nullptr) return;
  if (m_currentAmbience->is_playing())
  {
    m_currentAmbience->toggle_reverb(true);
  }
}

void RPAmbiencePlayer::stop()
{
  if(m_lastAmbience)
    m_lastAmbience->stop();
  if(m_currentAmbience)
    m_currentAmbience->stop();
}
