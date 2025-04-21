#include "drcharactermovie.h"

#include "aoapplication.h"
#include "dro/fs/fs_reading.h"

#include <QDebug>

DRCharacterMovie::DRCharacterMovie(AOApplication *ao_app, QGraphicsItem *parent)
    : DRMovie(parent)
    , ao_app(ao_app)
{
  Q_ASSERT(ao_app);
  set_scaling_mode(ScalingMode::HeightScaling);
}

DRCharacterMovie::~DRCharacterMovie()
{}

void DRCharacterMovie::play(QString p_character, QString p_emote, QString p_prefix, bool p_use_placeholder, bool p_play_once)
{
  set_file_name(ao_app->get_character_sprite_path(p_character, p_emote, p_prefix, p_use_placeholder));
  set_play_once(p_play_once);
  start();
}

void DRCharacterMovie::play_pre(QString p_character, QString p_emote)
{
  play(p_character, p_emote, nullptr, false, true);
}

void DRCharacterMovie::play_talk(QString p_character, QString p_emote)
{
  play(p_character, p_emote, "(b)", true, false);
}

void DRCharacterMovie::play_idle(QString p_character, QString p_emote)
{
  play(p_character, p_emote, "(a)", true, false);
}
