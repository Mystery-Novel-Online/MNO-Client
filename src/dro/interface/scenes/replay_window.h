#ifndef REPLAY_WINDOW_H
#define REPLAY_WINDOW_H
#include <QSlider>
#include <QWidget>
#include "dro/interface/widgets/rp_hover_widget.h"

class RPViewport;
class RPButton;

enum ReplayState
{
  ReplayState_Manual,
  ReplayState_Automatic,
  ReplayState_Realtime
};

class ReplayWindow : public QWidget
{
  Q_OBJECT
public:
  ReplayWindow();
  void setScrubberData(int length);
  void setScrubberPosition(int position);
  void setState(ReplayState state);

private:
  void constructLayout();

public slots:
  void onAutoToggle();
  void onTextComplete();
  void onScrubberPressed();
  void onScrubberReleased();
  void onScrubberValue();

private:
  ReplayState m_currentState = ReplayState_Manual;
  RPButton *m_autoToggle = nullptr;
  RPViewport *m_viewport = nullptr;
  RPHoverWidget *m_playbackHover = nullptr;
  RPHoverWidget *m_scrubberHover = nullptr;
  QSlider *m_playbackScrubber = nullptr;
  bool m_DraggingSlider = false;

protected:
  void keyPressEvent(QKeyEvent *event) override;
};

#endif // REPLAY_WINDOW_H
