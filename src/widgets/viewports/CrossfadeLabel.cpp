#include "CrossfadeLabel.h"

CrossfadeLabel::CrossfadeLabel(AOApplication *aoApp, QWidget* parent) : RPLabel(parent, aoApp)
{
}

void CrossfadeLabel::assignRenderTarget(DRGraphicsView *viewport)
{
  m_renderTarget = viewport;
}

void CrossfadeLabel::render()
{
  if(!m_renderTarget) {
    return;
  }

  QImage image(m_renderTarget->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
  image.fill(Qt::transparent);

  QPainter painter(&image);
  m_renderTarget->scene()->render(&painter);
  setPixmap(QPixmap::fromImage(image));
}

void CrossfadeLabel::animate()
{
  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect();
  setGraphicsEffect(eff);

  QPropertyAnimation *a = new QPropertyAnimation(eff, "opacity");
  a->setDuration(m_fadeDuration);
  a->setStartValue(1);
  a->setEndValue(0);
  a->setEasingCurve(QEasingCurve::OutBack);
  a->start(QPropertyAnimation::DeleteWhenStopped);
}
