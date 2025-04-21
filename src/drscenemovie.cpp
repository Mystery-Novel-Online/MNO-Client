#include "drscenemovie.h"

#include "aoapplication.h"
#include "dro/fs/file_utils.h"

DRSceneMovie::DRSceneMovie(AOApplication *ao_app, QGraphicsItem *parent)
    : DRMovie(parent)
    , ao_app(ao_app)
{
  set_scaling_mode(ScalingMode::DynamicScaling);
}

DRSceneMovie::~DRSceneMovie()
{}

void DRSceneMovie::set_background_image(QString p_background_name, QString p_image)
{
  const QString l_filename = ao_app->get_background_sprite_path(p_background_name, p_image);
  if (l_filename == file_name())
  {
    return;
  }
  set_file_name(l_filename);
  start();
}
