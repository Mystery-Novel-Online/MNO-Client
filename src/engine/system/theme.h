#ifndef THEME_H
#define THEME_H

namespace engine::system::theme
{
  void reloadMetadata();

  void applyDimensions(QWidget *widget, const QString &identifier, ThemeSceneType scene, bool allowResize = true);

  RPRect getDimensions(const QString &identifier, ThemeSceneType scene);
  RPRect getPositionalDimensions(const QString &identifier, const QString &positional);

}


#endif // THEME_H
