#pragma once

#include <QOpenGLWidget>
#include "Renderer.h"
#include <imgui/ImGuiScene.h>

class FennecWidget : public QOpenGLWidget
{
  Q_OBJECT

public:
  explicit FennecWidget(QWidget* parent = nullptr);
  ~FennecWidget();

protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;

  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;

  void timerEvent(QTimerEvent*) override;

private:
  Renderer renderer;

  ImGuiScene m_defaultImGuiScene;
};
