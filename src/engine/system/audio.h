#ifndef AUDIO_H
#define AUDIO_H

#include "engine/system/audio/music_player.h"
#include <string>

namespace audio
{
  void Initialize();
  bool IsSuppressed();
  void Suppress(bool enabled);
  void StopAll();

  namespace effect
  {
    void Play(const std::string& name);
    void PlayCharacter(const std::string& character, const std::string& name);
    void PlayAmbient(const std::string& filepath);
    void PlayWeather(const std::string& filepath);
    void stopWeather();
    void StopAll();
  }

  namespace system
  {
    void Play(const char *name);
  }

  namespace shout
  {
    void Play(const std::string& character, const std::string& name);
  }

  namespace bgm
  {
    void Play(const std::string& name);
    void PlayMode(const std::string& name, BGMPlayback playbackType);
    void Stop();
    void SetSpeed(float speed);
    void SetPitch(float speed);
    void SetReverb(bool reverb);
  }

  namespace blip
  {
    int getBlipRate();
    void setBlipRate(int rate);

    void Tick();
    void SetSound(const char *sound);
    void SetGender(const char *gender);
  }

};

#endif // AUDIO_H
