#include "replay_window.h"

#include <QDebug>
#include <QKeyEvent>

#include "commondefs.h"
#include "dro/interface/widgets/rp_button.h"
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
  setState(ReplayState_Manual);
  m_playbackScrubber->setMinimum(0);
  m_playbackScrubber->setMaximum(length - 1);
}

void ReplayWindow::setScrubberPosition(int position)
{
  m_DraggingSlider = true;
  m_playbackScrubber->setSliderPosition(position);
  m_DraggingSlider = false;
}

void ReplayWindow::setState(ReplayState state)
{
  switch(state)
  {
  case ReplayState_Realtime:
    m_currentState = ReplayState_Realtime;
    m_autoToggle->setText("Realtime");
    dro::system::replays::playback::progressSingle();
    break;

  case ReplayState_Automatic:
    m_currentState = ReplayState_Automatic;
    dro::system::replays::playback::setNextUpdate(2000);
    m_autoToggle->setText("Automatic");
    break;

  case ReplayState_Manual:
  default:
    m_currentState = ReplayState_Manual;
    m_autoToggle->setText("Manual");
    dro::system::replays::playback::setNextUpdate(0);
    break;
  }
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

  m_autoToggle = new RPButton("auto", "", "Manual", this);
  m_autoToggle->move(824, 10);
  m_autoToggle->resize(116, 36);
  setState(ReplayState_Manual);

  connect(m_autoToggle, &QPushButton::clicked, this, &ReplayWindow::onAutoToggle);
  connect(m_playbackScrubber, &QAbstractSlider::sliderPressed, this, &ReplayWindow::onScrubberPressed);
  connect(m_playbackScrubber, &QAbstractSlider::sliderReleased, this, &ReplayWindow::onScrubberReleased);
  connect(m_playbackScrubber, &QAbstractSlider::valueChanged, this, &ReplayWindow::onScrubberValue);
  connect(m_viewport, &RPViewport::textDone, this, &ReplayWindow::onTextComplete);

}

void ReplayWindow::onAutoToggle()
{
  switch(m_currentState)
  {
    case ReplayState_Manual:
      setState(ReplayState_Automatic);
      break;

    case ReplayState_Automatic:
      setState(ReplayState_Realtime);
      break;

    case ReplayState_Realtime:
    default:
      setState(ReplayState_Manual);
      break;

  }
}

void ReplayWindow::onTextComplete()
{
  if(m_currentState != ReplayState_Automatic) return;
  dro::system::replays::playback::setNextUpdate(2000);

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
  {
    playback::progress();
  }
  else
    QWidget::keyPressEvent(event);
}
