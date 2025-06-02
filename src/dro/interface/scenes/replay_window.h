#ifndef REPLAY_WINDOW_H
#define REPLAY_WINDOW_H
#include <QSlider>
#include <QWidget>
#include "dro/interface/widgets/rp_hover_widget.h"

class RPViewport;

class ReplayWindow : public QWidget
{
  Q_OBJECT
public:
  ReplayWindow();
  void setScrubberData(int length);
  void setScrubberPosition(int position);

private:
  void constructLayout();

public slots:
  void onScrubberReleased();

private:
  RPViewport *m_viewport = nullptr;
  RPHoverWidget *m_scrubberHover = nullptr;
  QSlider *m_playbackScrubber = nullptr;

protected:
  void keyPressEvent(QKeyEvent *event) override;
};

#endif // REPLAY_WINDOW_H
