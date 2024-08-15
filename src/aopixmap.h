#ifndef AOPIXMAP_H
#define AOPIXMAP_H

#include <QBitmap>
#include <QPixmap>

class AOPixmap
{
public:
  AOPixmap(QPixmap p_pixmap = QPixmap());
  AOPixmap(QString p_file_path);

  void clear();

  QPixmap scale(QSize p_size);
  QPixmap scale_to_height(QSize p_size);

  void SetAlphaMask(int t_level);
  void SetAlphaBase(QString l_path, int t_level);

private:
  QPixmap m_pixmap;
  QPixmap m_AlphaBase;

};

#endif
