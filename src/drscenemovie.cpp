#include "drscenemovie.h"
#include "pch.h"

DRSceneMovie::DRSceneMovie(AOApplication *ao_app, QGraphicsItem *parent)
    : DRMovie(parent)
    , ao_app(ao_app)
{
  set_scaling_mode(ScalingMode::DynamicScaling);
}

DRSceneMovie::~DRSceneMovie()
{}
