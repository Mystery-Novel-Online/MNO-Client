#include "CrossfadeLabel.h"

CrossfadeLabel::CrossfadeLabel(AOApplication *aoApp, QWidget* parent) : RPLabel(parent, aoApp)
{
  QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect();
  setGraphicsEffect(eff);

  m_fadeAnimation = new QPropertyAnimation(eff, "opacity");
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
  m_fadeAnimation->setDuration(m_fadeDuration);
  m_fadeAnimation->setStartValue(1);
  m_fadeAnimation->setEndValue(0);
  m_fadeAnimation->setEasingCurve(QEasingCurve::OutBack);
  m_fadeAnimation->start();
}
