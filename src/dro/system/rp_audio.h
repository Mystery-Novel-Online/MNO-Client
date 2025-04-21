#ifndef RP_AUDIO_H
#define RP_AUDIO_H

#include <QString>


class RPAudio
{
public:
  static void Initialize();
  static void PlayEffect(const QString &name);
  static void PlayBlip();
};

#endif // RP_AUDIO_H
