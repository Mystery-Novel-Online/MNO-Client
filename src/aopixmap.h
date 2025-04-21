#ifndef AOPIXMAP_H
#define AOPIXMAP_H

#include <QPixmap>

class AOPixmap
{
public:
  AOPixmap(QPixmap p_pixmap = QPixmap());
  AOPixmap(QString p_file_path);

  void clear();

  QPixmap scale(QSize p_size);
  QPixmap scale_to_height(QSize p_size);

private:
  QPixmap m_pixmap;
};

#endif
