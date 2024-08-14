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

  void SetAlphaMask(QString l_path, int x, int y);
  void SetAlphaBase(QString l_path);
  void UpdateAlphaCords(int x, int y);

private:
  QPixmap m_pixmap;
  QPixmap m_AlphaBase;
  QBitmap m_AlphaMaskImage;
  QBitmap m_AlphaMaskFilled;
};

#endif
