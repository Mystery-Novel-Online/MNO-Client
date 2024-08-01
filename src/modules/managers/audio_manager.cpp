#include "audio_manager.h"
#include "aoapplication.h"
#include <QRegularExpression>
#include <aomusicplayer.h>
AudioManager AudioManager::s_Instance;

void AudioManager::InitializeAudio()
{
  m_PlayerMusic = new AOMusicPlayer(AOApplication::getInstance());
  m_PlayerSFX = new AOSfxPlayer(AOApplication::getInstance());
  m_PlayerWeather = new AOSfxPlayer(AOApplication::getInstance());
}

void AudioManager::WeatherPlay(QString t_weather)
{
  QString l_filepath = AOApplication::getInstance()->get_ambient_sfx_path(t_weather);
  m_PlayerWeather->play_ambient(l_filepath);
}

void AudioManager::BGMPlay(QString t_track)
{
  QRegularExpression urlRegex("(http|https|ftp)://[^\\s/$.?#].[^\\s]*");
  QRegularExpressionMatch match = urlRegex.match(t_track);

  if (match.hasMatch())
  {
    m_PlayerMusic->play_streamed(t_track);
  }
  else
  {
    m_PlayerMusic->play(t_track);
  }
}

void AudioManager::PlaySFX(QString t_sfx)
{
  m_PlayerSFX->play_effect(t_sfx);
}

void AudioManager::PlaySFXCharacter(QString t_sfx, QString t_character)
{
  m_PlayerSFX->play_character_effect(t_character, t_sfx);
}
