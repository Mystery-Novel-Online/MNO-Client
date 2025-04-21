#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QThread>
#include <QObject>
#include <AOPixmap.h>
#include "QDebug"

class ImageLoader : public QObject
{
  Q_OBJECT
public:
  explicit ImageLoader(QObject *parent = nullptr);

  AOPixmap loadPixmap();


public slots:
  void loadImage(const QString& imagePath);

signals:
  void LoadingComplete(AOPixmap &pixmap);

private:
  AOPixmap loaderResult;
};

#endif // IMAGELOADER_H
