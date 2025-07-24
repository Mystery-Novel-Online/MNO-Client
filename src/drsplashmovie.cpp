#include "drsplashmovie.h"

DRSplashMovie::DRSplashMovie(AOApplication *ao_app, QGraphicsItem *parent)
    : DRMovie(parent)
    , ao_app(ao_app)
{
  set_play_once(true);
  set_hide_on_done(true);
}

DRSplashMovie::~DRSplashMovie()
{}

void DRSplashMovie::play(QString p_file_name, QString p_character)
{
  set_file_name(ao_app->get_wtce_anim_path(p_file_name));
  start();
}

void DRSplashMovie::play(QString p_file_name)
{
  play(p_file_name, "");
}
