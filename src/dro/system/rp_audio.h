#ifndef RP_AUDIO_H
#define RP_AUDIO_H

#include <QString>

namespace RPAudio
{
  void Initialize();
  void PlayEffect(const char *name);
  void PlaySystem(const char *name);
  void PlayBGM(const char *name);
  void SetBGMSpeed(float speed);
  void SetBGMPitch(float speed);
  void SetBGMReverb(bool reverb);

  void SetBlipSound(const char *sound);
  void SetBlipGender(const char *gender);
  void BlipTick();
};

#endif // RP_AUDIO_H
