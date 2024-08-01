#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <aomusicplayer.h>
#include <QString>
#include <aoapplication.h>
#include <aosfxplayer.h>

class AudioManager
{
public:
  static AudioManager& get()
  {
    return s_Instance;
  }

  void InitializeAudio();
  void WeatherPlay(QString t_weather);
  void BGMPlay(QString t_track);
  void PlaySFX(QString t_sfx);
  void PlaySFXCharacter(QString t_sfx, QString t_character);

private:
  AudioManager()
  {
  }
  static AudioManager s_Instance;

  AOMusicPlayer *m_PlayerMusic = nullptr;
  AOSfxPlayer *m_PlayerSFX = nullptr;
  AOSfxPlayer *m_PlayerWeather = nullptr;
};

#endif // AUDIO_MANAGER_H
