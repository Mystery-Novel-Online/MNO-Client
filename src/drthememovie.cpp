#include "drthememovie.h"

DRThemeMovie::DRThemeMovie(AOApplication *app, QGraphicsItem *parent)
    : DRMovie(parent)
    , ao_app(app)
{}

DRThemeMovie::~DRThemeMovie()
{}

void DRThemeMovie::set_theme_image(QString p_image)
{
  set_file_name(ao_app->find_theme_asset_path(p_image));
  start();
}
