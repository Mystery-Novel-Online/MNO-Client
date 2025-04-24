#ifndef RP_AUDIO_H
#define RP_AUDIO_H

#include <QString>

namespace RPAudio
{
  void Initialize();
  void PlayEffect(const QString &name);
  void PlayBlip();
};

#endif // RP_AUDIO_H
