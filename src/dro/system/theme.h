#ifndef THEME_H
#define THEME_H

namespace dro::system::theme
{
  void reloadMetadata();

  void applyDimensions(QWidget *widget, const QString &identifier, ThemeSceneType scene, bool allowResize = true);

  pos_size_type getDimensions(const QString &identifier, ThemeSceneType scene);
  pos_size_type getPositionalDimensions(const QString &identifier, const QString &positional);

}


#endif // THEME_H
