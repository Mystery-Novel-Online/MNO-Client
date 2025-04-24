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

  void PlaySystem(const char *name)
  {
    s_systemPlayer->play(name);
  }

  void PlayBGM(const char *name)
  {
    s_musicPlayer->play(name);
  }

  void SetBlipGender(const char *gender)
  {
    s_blipPlayer->set_blips("sfx-blip" + QString(gender) + ".wav");
  }

  void BlipTick()
  {
    s_blipPlayer->blip_tick();
  }

  void SetBlipSound(const char *sound)
  {
    s_blipPlayer->set_blips(sound);
  }

  void SetBGMSpeed(float speed)
  {
    s_musicPlayer->setSpeed(speed);
  }

  void SetBGMPitch(float pitch)
  {
    s_musicPlayer->setPitch(pitch);
  }

}
