#include "image_loader.h"

ImageLoader::ImageLoader(QObject *parent) : QObject(parent)
{

}


void ImageLoader::loadImage(const QString &imagePath)
{
  AOPixmap pixmap(imagePath);

  loaderResult = pixmap;

  emit LoadingComplete(pixmap);
}
