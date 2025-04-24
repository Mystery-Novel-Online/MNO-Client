#include "rp_audio.h"
#include "aomusicplayer.h"

#include <aoapplication.h>
#include <aosfxplayer.h>

static AOMusicPlayer *s_musicPlayer;
static AOSfxPlayer *s_effectsPlayer;

namespace RPAudio
{

  void PlayBlip()
  {

  }

  void Initialize()
  {
    s_musicPlayer = new AOMusicPlayer(AOApplication::getInstance());
    s_effectsPlayer = new AOSfxPlayer(AOApplication::getInstance());
  }

  void PlayEffect(const QString &effectName)
  {
    s_effectsPlayer->play_effect(effectName);
  }

}
