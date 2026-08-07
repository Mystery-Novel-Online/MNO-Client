#pragma once

#include <QOpenGLWidget>
#include "Renderer.h"
#include <imgui/ImGuiScene.h>

class FennecWidget : public QOpenGLWidget
{
  Q_OBJECT

public:
  explicit FennecWidget(QWidget* parent = nullptr);

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;

  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;

  void timerEvent(QTimerEvent*) override;

private:
  Renderer renderer;

  ImGuiScene m_defaultImGuiScene;
};
