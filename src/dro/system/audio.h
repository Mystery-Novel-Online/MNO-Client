#ifndef AUDIO_H
#define AUDIO_H

#include <string>


namespace audio
{
  void Initialize();

  namespace effect
  {
    void Play(const char *name);
  }

  namespace system
  {
    void Play(const char *name);
  }

  namespace bgm
  {
    void Play(const std::string& name);
    void Stop();
    void SetSpeed(float speed);
    void SetPitch(float speed);
    void SetReverb(bool reverb);
  }

  namespace blip
  {
    void Tick();
    void SetSound(const char *sound);
    void SetGender(const char *gender);
  }

};

#endif // AUDIO_H
