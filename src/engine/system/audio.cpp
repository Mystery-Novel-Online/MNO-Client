#include "audio.h"

#include "engine/system/audio/blip_player.h"
#include "engine/system/audio/music_player.h"
#include "engine/system/audio/sfx_player.h"
#include "engine/system/audio/system_player.h"
#include "engine/system/audio/shout_player.h"
#include "engine/system/audio/ambience_player.h"


static AOBlipPlayer *s_blipPlayer;
static int s_blipRateOverride = -1;

static AOMusicPlayer *s_musicPlayer;
static AOSfxPlayer *s_effectsPlayer;
static RPAmbiencePlayer *s_weatherPlayer;
static AOShoutPlayer *s_shoutPlayer;
static AOSystemPlayer *s_systemPlayer;

bool s_AudioMuted = false;

namespace audio
{

  void Initialize()
  {
    s_musicPlayer = new AOMusicPlayer();
    s_effectsPlayer = new AOSfxPlayer();
    s_weatherPlayer = new RPAmbiencePlayer();
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

    void PlayWeather(const std::string &filepath)
    {
      s_weatherPlayer->play(QString::fromStdString(filepath));
    }

    void StopAll()
    {
      s_effectsPlayer->stop_all();
    }

    void stopWeather()
    {
      s_weatherPlayer->stop();
    }

  }

  namespace system
  {
    void Play(const std::string& name)
    {
      s_systemPlayer->play(QString::fromStdString(name));
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

    void PlayMode(const std::string &name, BGMPlayback playbackType)
    {
      s_musicPlayer->play(QString::fromStdString(name), playbackType);
    }

  }

  namespace blip
  {
    void Tick()
    {
      s_blipPlayer->blip_tick();
    }

    void SetGender(const std::string& gender)
    {
      s_blipPlayer->set_blips("sfx-blip" + QString::fromStdString(gender) + ".wav");
    }

    void SetSound(const std::string& sound)
    {
      s_blipPlayer->set_blips(QString::fromStdString(sound));
    }

    int getBlipRate()
    {
      return s_blipRateOverride;
    }

    void setBlipRate(int rate)
    {
      s_blipRateOverride = rate;
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
