#include "dro_camera_display.h"

DROCameraDisplay::DROCameraDisplay(QWidget *parent) : QWidget{parent}
{
  setWindowTitle("GM Camera Display");
}

void DROCameraDisplay::SetCameraCount(int t_cameraColumns, int t_cameraRows)
{
  m_CameraCount = t_cameraRows * t_cameraColumns;

  for(DROViewportWidget * l_viewport : m_CameraDisplays)
    delete l_viewport;

  m_CameraDisplays.clear();

  int l_CamWidth = 960 * m_CameraScale;
  int l_CamHeight = 544 * m_CameraScale;

  this->resize(l_CamWidth * t_cameraColumns, l_CamHeight * t_cameraRows);

  for(int y = 0; y < t_cameraRows; y++)
  {
    for(int x = 0; x < t_cameraColumns; x++)
    {
      DROViewportWidget *l_newViewport = new DROViewportWidget(this, m_CameraScale);
      l_newViewport->resize(l_CamWidth, l_CamHeight);
      l_newViewport->move(x * l_CamWidth, y * l_CamHeight);
      l_newViewport->ConstructViewport(SceneTypeReplays);
      l_newViewport->show();
      m_CameraDisplays.append(l_newViewport);
    }
  }
}

void DROCameraDisplay::SetCameraBackground(int id, QString t_background)
{
  if(m_CameraDisplays.count() <= id) return;
  m_CameraDisplays.at(id)->SetBackground(t_background);
}

