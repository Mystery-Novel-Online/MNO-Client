#include "fennec_widget.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include "imgui_setup.h"
#include "imgui/SampleWindow.h"
#include "imgui/ImGuiScene.h"

FennecWidget::FennecWidget(QWidget* parent)
    : QOpenGLWidget(parent), m_defaultImGuiScene()
{
  m_defaultImGuiScene.createWindow<SampleWindow>();
  startTimer(16);
}

void FennecWidget::initializeGL()
{
  gladLoadGL();
  ImguiSetup::initQt();
  renderer.init();
}

void FennecWidget::paintGL()
{
  renderer.clear(0.2f, 0.3f, 0.4f);

  QPoint global = QCursor::pos();
  QPoint local = mapFromGlobal(global);

  m_defaultImGuiScene.render(ImVec2(local.x(), local.y()), ImVec2((float)width(), (float)height()));
}

void FennecWidget::resizeGL(int w, int h)
{
  renderer.resize(w, h);
}

void FennecWidget::mousePressEvent(QMouseEvent* e)
{
  ImGuiIO& io = ImGui::GetIO();

  if (e->button() == Qt::LeftButton)
    io.MouseDown[0] = true;
}

void FennecWidget::mouseReleaseEvent(QMouseEvent* e)
{
  ImGuiIO& io = ImGui::GetIO();

  if (e->button() == Qt::LeftButton)
    io.MouseDown[0] = false;
}

void FennecWidget::timerEvent(QTimerEvent* event)
{
  update();
}
