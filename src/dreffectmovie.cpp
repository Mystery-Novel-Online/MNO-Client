#include "dreffectmovie.h"

DREffectMovie::DREffectMovie(AOApplication *ao_app, QGraphicsItem *parent)
    : DRMovie(parent)
    , ao_app(ao_app)
{
  set_hide_on_done(true);
}

DREffectMovie::~DREffectMovie()
{}

void DREffectMovie::play(QString p_file_name, QString p_character)
{
  set_file_name(ao_app->get_theme_sprite_path(p_file_name, p_character));
  start();
}

void DREffectMovie::play(QString p_file_name)
{
  play(p_file_name, "");
}
