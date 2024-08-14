#include "aopixmap.h"

#include "aoapplication.h"

#include <qpainter.h>

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

void AOPixmap::SetAlphaMask(QString l_path, int x, int y)
{
  m_AlphaMaskImage.load(l_path);
  m_AlphaMaskFilled = QBitmap(m_pixmap.size());
  m_AlphaMaskFilled.fill(Qt::white);


  QPainter painter(&m_AlphaMaskFilled);
  QPoint l_AlphaCords(x, y);
  painter.drawPixmap(l_AlphaCords, m_AlphaMaskImage);

  m_pixmap.setMask(m_AlphaMaskFilled);
}

void AOPixmap::SetAlphaBase(QString l_path)
{
  m_AlphaBase = QPixmap(l_path);
  m_pixmap = QPixmap(m_AlphaBase);
  UpdateAlphaCords(9999, 9999);
}

void AOPixmap::UpdateAlphaCords(int x, int y)
{
  m_AlphaMaskFilled = QBitmap(m_pixmap.size());
  m_AlphaMaskFilled.fill(Qt::white);

  QPainter painter(&m_AlphaMaskFilled);
  QPoint l_AlphaCords(x, y);
  painter.drawPixmap(l_AlphaCords, m_AlphaMaskImage);

  m_pixmap = QPixmap(m_AlphaBase);
  m_pixmap.setMask(m_AlphaMaskFilled);
}
