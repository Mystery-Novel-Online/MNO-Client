#ifndef THEME_H
#define THEME_H

#include "datatypes.h"

namespace dro::system::theme
{
  void applyDimensions(QWidget *widget, QString identifier, ThemeSceneType scene);
  pos_size_type getDimensions(QString identifier, ThemeSceneType scene);

}


#endif // THEME_H
