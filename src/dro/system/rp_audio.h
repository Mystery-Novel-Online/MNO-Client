#ifndef RP_AUDIO_H
#define RP_AUDIO_H

#include <QString>

namespace RPAudio
{
  void Initialize();
  void PlayEffect(const char *name);
  void PlayBlip(const char *name);
  void PlaySystem(const char *name);
  void PlayBGM(const char *name);
};

#endif // RP_AUDIO_H
