#include "audio.h"
#include "aomusicplayer.h"

#include <aoapplication.h>
#include <aoblipplayer.h>
#include <aosfxplayer.h>
#include <aosystemplayer.h>

#include <QString>

static AOMusicPlayer *s_musicPlayer;
static AOSfxPlayer *s_effectsPlayer;
static AOSystemPlayer *s_systemPlayer;
static AOBlipPlayer *s_blipPlayer;


namespace audio
{

  void Initialize()
  {
    s_musicPlayer = new AOMusicPlayer(AOApplication::getInstance());
    s_effectsPlayer = new AOSfxPlayer(AOApplication::getInstance());
    s_systemPlayer = new AOSystemPlayer(AOApplication::getInstance());
    s_blipPlayer = new AOBlipPlayer(AOApplication::getInstance());
  }

  namespace effect
  {
    void Play(const char *name)
    {
      s_effectsPlayer->play_effect(name);
    }
  }

  namespace system
  {
    void Play(const char *name)
    {
      s_systemPlayer->play(name);
    }
  }

  namespace bgm
  {
    void Play(const std::string& name)
    {
      s_musicPlayer->play(QString::fromStdString(name));
    }

    void SetSpeed(float speed)
    {
      s_musicPlayer->setSpeed(speed);
    }

    void SetPitch(float pitch)
    {
      s_musicPlayer->setPitch(pitch);
    }

    void SetReverb(bool reverb)
    {
      s_musicPlayer->toggleReverb(reverb);
    }

    void Stop()
    {
      s_musicPlayer->stop();
    }

  }

  namespace blip
  {
    void Tick()
    {
      s_blipPlayer->blip_tick();
    }

    void SetGender(const char *gender)
    {
      s_blipPlayer->set_blips("sfx-blip" + QString(gender) + ".wav");
    }

    void SetSound(const char *sound)
    {
      s_blipPlayer->set_blips(sound);
    }
  }





}
