#pragma once

#include "drmovie.h"

class AOApplication;

class DRSceneMovie : public DRMovie
{
  Q_OBJECT

public:
  explicit DRSceneMovie(AOApplication *ao_app, QGraphicsItem *parent = nullptr);
  ~DRSceneMovie();

private:
  AOApplication *ao_app;
};
