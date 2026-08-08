#include "fennec_widget.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

#include "imgui_setup.h"
#include "imgui/SampleWindow.h"
#include "imgui/ImGuiScene.h"

#include <fennec/windows/NetworkWindow.h>
#include <fennec/windows/ThemeWindow.h>


static ImGuiKey qtToImguiKey(int key)
{
  if (key >= Qt::Key_A && key <= Qt::Key_Z)
    return static_cast<ImGuiKey>(ImGuiKey_A + (key - Qt::Key_A));

  if (key >= Qt::Key_0 && key <= Qt::Key_9)
    return static_cast<ImGuiKey>(ImGuiKey_0 + (key - Qt::Key_0));

  switch (key)
  {
    case Qt::Key_Tab:       return ImGuiKey_Tab;
    case Qt::Key_Left:      return ImGuiKey_LeftArrow;
    case Qt::Key_Right:     return ImGuiKey_RightArrow;
    case Qt::Key_Up:        return ImGuiKey_UpArrow;
    case Qt::Key_Down:      return ImGuiKey_DownArrow;
    case Qt::Key_Backspace: return ImGuiKey_Backspace;
    case Qt::Key_Delete:    return ImGuiKey_Delete;
    case Qt::Key_Enter:
    case Qt::Key_Return:    return ImGuiKey_Enter;
    case Qt::Key_Escape:    return ImGuiKey_Escape;
    case Qt::Key_Space:     return ImGuiKey_Space;

    default:
      return ImGuiKey_None;
  }
}

FennecWidget::FennecWidget(QWidget* parent)
    : QOpenGLWidget(parent), m_defaultImGuiScene()
{
  m_defaultImGuiScene.createWindow<ThemeWindow>();
  m_defaultImGuiScene.createWindow<NetworkWindow>();
  startTimer(16);
}

FennecWidget::~FennecWidget()
{
  makeCurrent();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui::DestroyContext();

  doneCurrent();
}

void FennecWidget::initializeGL()
{
  gladLoadGL();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  ImguiSetup::initQt();
  renderer.init();
}

void FennecWidget::paintGL()
{
  renderer.clear(0.2f, 0.3f, 0.4f, 0.0f);

  QPoint global = QCursor::pos();
  QPoint local = mapFromGlobal(global);

  m_defaultImGuiScene.render(ImVec2(local.x(), local.y()), ImVec2((float)width(), (float)height()));
}

void FennecWidget::resizeGL(int w, int h)
{
  renderer.resize(w, h);
}

void FennecWidget::keyPressEvent(QKeyEvent *event)
{
  ImGuiIO& io = ImGui::GetIO();

  ImGuiKey key = qtToImguiKey(event->key());

  if (key != ImGuiKey_None)
    io.AddKeyEvent(key, true);

  const QString text = event->text();

  for (const QChar& character : text)
  {
    if (!character.isNull())
      io.AddInputCharacter(character.unicode());
  }

  event->accept();
}

void FennecWidget::keyReleaseEvent(QKeyEvent *event)
{
  ImGuiIO& io = ImGui::GetIO();

  ImGuiKey key = qtToImguiKey(event->key());

  if (key != ImGuiKey_None)
    io.AddKeyEvent(key, false);

  event->accept();
}

void FennecWidget::mousePressEvent(QMouseEvent* e)
{
  this->setFocus();
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
