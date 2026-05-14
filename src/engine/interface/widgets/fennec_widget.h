#pragma once

#include <QOpenGLWidget>
#include "Renderer.h"

class FennecWidget : public QOpenGLWidget
{
  Q_OBJECT

public:
  explicit FennecWidget(QWidget* parent = nullptr);

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;

  void timerEvent(QTimerEvent*) override;

private:
  Renderer renderer;
};
