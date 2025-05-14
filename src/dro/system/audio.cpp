#include "audio.h"
#include "aomusicplayer.h"

#include <aoapplication.h>
#include <aoblipplayer.h>
#include <aosfxplayer.h>
#include <aosystemplayer.h>
#include "aoshoutplayer.h"

#include <QString>

static AOMusicPlayer *s_musicPlayer;
static AOSfxPlayer *s_effectsPlayer;
static AOSystemPlayer *s_systemPlayer;
static AOBlipPlayer *s_blipPlayer;
static AOShoutPlayer *s_shoutPlayer;

bool s_AudioMuted = false;

namespace audio
{

  void Initialize()
  {
    s_musicPlayer = new AOMusicPlayer();
    s_effectsPlayer = new AOSfxPlayer();
    s_systemPlayer = new AOSystemPlayer();
    s_blipPlayer = new AOBlipPlayer();
    s_shoutPlayer = new AOShoutPlayer();
  }

  bool IsSuppressed()
  {
    return s_AudioMuted;
  }

  void Suppress(bool enabled)
  {
    s_AudioMuted = enabled;

    for (auto &family : DRAudioEngine::get_family_list())
    {
      family->set_suppressed(s_AudioMuted);
    }
  }

  void StopAll()
  {
    for (auto &family : DRAudioEngine::get_family_list())
      for (auto &stream : family->get_stream_list())
        stream->stop();
  }

  namespace effect
  {
    void Play(const std::string& name)
    {
      s_effectsPlayer->play_effect(QString::fromStdString(name));
    }

    void PlayCharacter(const std::string& character, const std::string& name)
    {
      s_effectsPlayer->play_character_effect(QString::fromStdString(character), QString::fromStdString(name));
    }

    void PlayAmbient(const std::string &filepath)
    {
      s_effectsPlayer->play_ambient(QString::fromStdString(filepath));
    }

    void StopAll()
    {
      s_effectsPlayer->stop_all();
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

  namespace shout
  {
    void Play(const std::string& character, const std::string& name)
    {
      s_shoutPlayer->play(QString::fromStdString(character), QString::fromStdString(name));
    }
  }

}
