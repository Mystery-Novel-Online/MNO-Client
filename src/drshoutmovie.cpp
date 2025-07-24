#include "drshoutmovie.h"
#include "pch.h"

DRShoutMovie::DRShoutMovie(AOApplication *ao_app, QGraphicsItem *parent)
    : DRMovie(parent)
    , ao_app(ao_app)
{
  Q_ASSERT(ao_app);
  set_play_once(true);
  set_hide_on_done(true);
}

DRShoutMovie::~DRShoutMovie()
{}

void DRShoutMovie::play_interjection(QString p_character, QString p_shout)
{
  set_file_name(ao_app->get_shout_sprite_path(p_character, p_shout));
  set_play_once(true);
  start();
}
