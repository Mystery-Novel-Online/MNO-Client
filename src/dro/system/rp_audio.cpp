#include "rp_audio.h"
#include "aomusicplayer.h"

#include <aoapplication.h>
#include <aoblipplayer.h>
#include <aosfxplayer.h>
#include <aosystemplayer.h>

static AOMusicPlayer *s_musicPlayer;
static AOSfxPlayer *s_effectsPlayer;
static AOSystemPlayer *s_systemPlayer;
static AOBlipPlayer *s_blipPlayer;


namespace RPAudio
{

  void Initialize()
  {
    s_musicPlayer = new AOMusicPlayer(AOApplication::getInstance());
    s_effectsPlayer = new AOSfxPlayer(AOApplication::getInstance());
    s_systemPlayer = new AOSystemPlayer(AOApplication::getInstance());
    s_blipPlayer = new AOBlipPlayer(AOApplication::getInstance());
  }

  void PlayEffect(const char *name)
  {
    s_effectsPlayer->play_effect(name);
  }

  void PlayBlip(const char *name)
  {
    //s_blipPlayer->blip_tick()name);
  }

  void PlaySystem(const char *name)
  {
    s_systemPlayer->play(name);
  }

  void PlayBGM(const char *name)
  {
    s_musicPlayer->play(name);
  }

}
