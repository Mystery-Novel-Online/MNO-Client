#include "replay_window.h"

#include "engine/system/replay_playback.h"
#include "theme.h"

using namespace engine::system;

ReplayWindow::ReplayWindow() : SceneWidget(ThemeSceneType::SceneType_Replays)
{
  setWindowTitle("Replay");
  setFocusPolicy(Qt::StrongFocus);
  constructLayout();

  replays::assignWindow(this);
  replays::assignViewport(m_viewport);
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
    m_autoToggle->set_image("playback_realtime.png");
    engine::system::replays::playback::progressSingle();
    break;

  case ReplayState_Automatic:
    m_currentState = ReplayState_Automatic;
    engine::system::replays::playback::setNextUpdate(2500);
    m_autoToggle->set_image("playback_auto.png");
    break;

  case ReplayState_Manual:
  default:
    m_currentState = ReplayState_Manual;
    m_autoToggle->set_image("playback_manual.png");
    engine::system::replays::playback::setNextUpdate(0);
    break;
  }

  this->setFocus();
}

void ReplayWindow::constructLayout()
{
  m_viewport = createWidget<LegacyViewport>("replay_window");
  m_viewport->constructViewport();
  m_playbackHover = createWidget<RPHoverWidget>("playback_hover_field");
  m_scrubberHover = createWidget<RPHoverWidget>("controller");
  m_playbackScrubber = createWidget<QSlider>("scrubber", Qt::Horizontal);
  m_scrubberHover->addWidget(m_playbackScrubber);

  m_autoToggle = createWidget<RPButton>("playback_mode", "auto", "", "Manual");

  m_playbackHover->addWidget(m_autoToggle);
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
  replays::playback::setNextUpdate(2500);

}

void ReplayWindow::onScrubberPressed()
{
  m_DraggingSlider = true;
}

void ReplayWindow::onScrubberReleased()
{
  m_DraggingSlider = false;
  replays::playback::setTimestamp(m_playbackScrubber->value());
}

void ReplayWindow::onScrubberValue()
{
  if(!m_DraggingSlider) replays::playback::setTimestamp(m_playbackScrubber->value());
}

void ReplayWindow::keyPressEvent(QKeyEvent *event)
{
  if (event->key() == Qt::Key_Space)
  {
    replays::playback::progress();
  }
  else
    QWidget::keyPressEvent(event);
}
