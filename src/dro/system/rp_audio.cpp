#include "rp_audio.h"
#include "aomusicplayer.h"

#include <aoapplication.h>
#include <aosfxplayer.h>

static AOMusicPlayer *s_musicPlayer;
static AOSfxPlayer *s_effectsPlayer;

void RPAudio::Initialize()
{
  s_musicPlayer = new AOMusicPlayer(AOApplication::getInstance());
  s_effectsPlayer = new AOSfxPlayer(AOApplication::getInstance());
}

void RPAudio::PlayEffect(const QString &effectName)
{
  s_effectsPlayer->play_effect(effectName);
}
