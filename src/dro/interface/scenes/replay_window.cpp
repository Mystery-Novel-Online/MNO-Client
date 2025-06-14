#include "replay_window.h"

#include <QDebug>
#include <QKeyEvent>

#include "commondefs.h"
#include "dro/interface/widgets/viewports/legacy_viewport.h"
#include "dro/system/replay_playback.h"
#include "theme.h"

using namespace dro::system::replays;

ReplayWindow::ReplayWindow()
{
  setWindowTitle("Replay");
  setFocusPolicy(Qt::StrongFocus);
  constructLayout();

  assignWindow(this);
  assignViewport(m_viewport);
}

void ReplayWindow::setScrubberData(int length)
{
  m_playbackScrubber->setMinimum(0);
  m_playbackScrubber->setMaximum(length - 1);
}

void ReplayWindow::setScrubberPosition(int position)
{
  m_playbackScrubber->setSliderPosition(position);
}

void ReplayWindow::constructLayout()
{
  m_viewport = new LegacyViewport(this);
  set_size_and_pos(this, "replay_window", REPLAY_DESIGN_INI, AOApplication::getInstance());
  m_viewport->constructViewport();

  m_scrubberHover = new RPHoverWidget(this);
  set_size_and_pos(m_scrubberHover, "controller", REPLAY_DESIGN_INI, AOApplication::getInstance());
  m_scrubberHover->show();


  m_playbackScrubber = new QSlider(Qt::Horizontal, this);
  set_size_and_pos(m_playbackScrubber, "scrubber", REPLAY_DESIGN_INI, AOApplication::getInstance());
  m_scrubberHover->addWidget(m_playbackScrubber);


  connect(m_playbackScrubber, SIGNAL(sliderPressed()), this, SLOT(onScrubberPressed()));
  connect(m_playbackScrubber, SIGNAL(sliderReleased()), this, SLOT(onScrubberReleased()));
  connect(m_playbackScrubber, SIGNAL(valueChanged(int)), this, SLOT(onScrubberValue()));

}

void ReplayWindow::onScrubberPressed()
{
  m_DraggingSlider = true;
}

void ReplayWindow::onScrubberReleased()
{
  m_DraggingSlider = false;
  playback::setTimestamp(m_playbackScrubber->value());
}

void ReplayWindow::onScrubberValue()
{
  if(!m_DraggingSlider) playback::setTimestamp(m_playbackScrubber->value());
}

void ReplayWindow::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Space)
    playback::progress();
  else
    QWidget::keyPressEvent(event);
}
