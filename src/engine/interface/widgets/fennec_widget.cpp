#include "fennec_widget.h"


FennecWidget::FennecWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
  startTimer(16);
}

void FennecWidget::initializeGL()
{
  gladLoadGL();

  renderer.init();
}

void FennecWidget::paintGL()
{
  renderer.clear(0.2f, 0.3f, 0.4f);
}

void FennecWidget::resizeGL(int w, int h)
{
  renderer.resize(w, h);
}

void FennecWidget::timerEvent(QTimerEvent*)
{
  update();
}
