#include "aopixmap.h"

AOPixmap::AOPixmap(QPixmap p_pixmap)
    : m_pixmap(p_pixmap)
{
  if (m_pixmap.isNull())
  {
    clear();
  }
}

AOPixmap::AOPixmap(QString p_file_path)
    : AOPixmap(QPixmap(p_file_path))
{}

void AOPixmap::clear()
{
  m_pixmap = QPixmap(1, 1);
  m_pixmap.fill(Qt::transparent);
}

QPixmap AOPixmap::scale(QSize p_size)
{
  const bool l_pixmap_is_larger = m_pixmap.width() > p_size.width() || m_pixmap.height() > p_size.height();
  const Qt::TransformationMode f_mode = l_pixmap_is_larger ? Qt::SmoothTransformation : Qt::FastTransformation;
  return m_pixmap.scaled(p_size, Qt::IgnoreAspectRatio, f_mode);
}

QPixmap AOPixmap::scale_to_height(QSize p_size)
{
  const bool l_pixmap_is_larger = m_pixmap.width() > p_size.width() || m_pixmap.height() > p_size.height();
  return m_pixmap.scaledToHeight(p_size.height(),
                                 l_pixmap_is_larger ? Qt::SmoothTransformation : Qt::FastTransformation);
}
