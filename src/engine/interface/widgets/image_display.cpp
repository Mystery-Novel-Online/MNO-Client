#include "aopixmap.h"
#include "engine/fs/fs_reading.h"
#include "modules/theme/thememanager.h"

/*!
 * @class AOImageDisplay
 * @brief Represents a static theme-dependent image.
 */
AOImageDisplay::AOImageDisplay(QWidget *parent, AOApplication *p_ao_app)
    : QLabel(parent)
{
  ao_app = p_ao_app;
}

AOImageDisplay::AOImageDisplay(QWidget *parent) : QLabel(parent)
{
  ao_app = AOApplication::getInstance();
}

QString AOImageDisplay::get_image()
{
  return m_image;
}

void AOImageDisplay::set_image(QString p_image)
{
  m_image = p_image;
  refreshImage();
}

void AOImageDisplay::refreshImage()
{
  if(!ThemeManager::get().mCurrentThemeReader.IsPixmapExist(m_image))
  {
    AOPixmap l_pixmap(m_image);

    const QString alphaMaskPath = AOApplication::getInstance()->find_theme_asset_path(m_alphaName + ".png");
    if (FS::Checks::FileExists(alphaMaskPath))
    {
      QImage base = l_pixmap.m_pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
      QImage mask(alphaMaskPath);

      if (!mask.isNull())
      {
        mask = mask.scaled(base.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32_Premultiplied);

        QPainter p(&base);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawImage(0, 0, mask);
        p.end();

        l_pixmap.m_pixmap = QPixmap::fromImage(base);
      }
    }


    setPixmap(l_pixmap.scale(size()));
  }
  else
  {
    setPixmap(ThemeManager::get().mCurrentThemeReader.GetCachedPixmap(m_image).scale(size()));
  }
}

void AOImageDisplay::set_theme_image(QString p_image)
{
  set_image(ao_app->find_theme_asset_path(p_image));
}

void AOImageDisplay::set_chatbox_image(QString p_chatbox_name, bool p_is_self)
{
  QString l_target_file = ao_app->find_asset_path(FS::Paths::BasePath() + "misc/" + p_chatbox_name + ".png");
  if (l_target_file.isEmpty())
  {
    l_target_file = ao_app->find_theme_asset_path("chatmed.png");

    if (p_is_self)
    {
      const QString l_self_file = ao_app->find_theme_asset_path("chatmed_self.png");
      if (!l_self_file.isEmpty())
      {
        l_target_file = l_self_file;
      }
    }
  }

  if (l_target_file.isEmpty())
  {
    qWarning() << "warning: could not retrieve any chatbox image, will display blank";
  }
  set_image(l_target_file);
}
