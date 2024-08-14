#ifndef DRO_CAMERA_DISPLAY_H
#define DRO_CAMERA_DISPLAY_H

#include <QObject>
#include <QWidget>
#include <modules/widgets/droviewportwidget.h>

class DROCameraDisplay : public QWidget
{
  Q_OBJECT
public:
  explicit DROCameraDisplay(QWidget *parent = nullptr);
  void SetCameraCount(int t_cameraRows, int t_cameraColumns);
  void SetCameraBackground(int id, QString t_background);

  void ProcessMessage(int l_ID, ICMessageData *l_MessageData)
  {
    if(m_CameraDisplays.count() <= l_ID) return;
    m_CameraDisplays.at(l_ID)->ProcessIncomingMessage(l_MessageData);
  }

signals:

private:
  int m_CameraCount = 0;
  double m_CameraScale = 0.5;
  QList<DROViewportWidget *> m_CameraDisplays = {};
};

#endif // DRO_CAMERA_DISPLAY_H
