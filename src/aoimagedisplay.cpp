#include "aoimagedisplay.h"

#include <QDebug>

#include "aoapplication.h"
#include "aopixmap.h"
#include "file_functions.h"
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

QString AOImageDisplay::get_image()
{
  return m_image;
}

void AOImageDisplay::set_image(QString p_image, bool t_forceRefresh)
{
  if(!t_forceRefresh)
  {
    if(m_image == p_image) return;
  }
  m_image = p_image;
  refreshImage();
}

void AOImageDisplay::refreshImage()
{
  if(m_Pixmap != nullptr)
  {
    delete m_Pixmap;
    m_Pixmap = nullptr;
  }
  if(!ThemeManager::get().mCurrentThemeReader.IsPixmapExist(m_image))
  {
    qDebug() << "[AOPixmap] Failed to find in theme, loading manually: " + m_image;
    m_Pixmap = new AOPixmap(m_image);
    setPixmap(m_Pixmap->scale(size()));
  }
  else
  {
    qDebug() << "[AOPixmap] Found in theme, loading: " + m_image;
    setPixmap(ThemeManager::get().mCurrentThemeReader.GetCachedPixmap(m_image).scale(size()));//->scale(size()));
  }
}

void AOImageDisplay::set_theme_image(QString p_image)
{
  set_image(ao_app->find_theme_asset_path(p_image));
}

void AOImageDisplay::set_chatbox_image(QString p_chatbox_name, bool p_is_self)
{
  QString l_target_file = ao_app->find_asset_path(ao_app->get_base_path() + "misc/" + p_chatbox_name + ".png");
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

void AOImageDisplay::SetImageBase(QString l_path, int l_level)
{
  m_alphaLevel = -1;
  if(m_Pixmap == nullptr)
  {
    m_Pixmap = new AOPixmap();
  };
  m_Pixmap->SetAlphaBase(l_path, l_level);
  setPixmap(m_Pixmap->scale(size()));
}

