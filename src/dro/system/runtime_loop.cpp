#include "runtime_loop.h"
#include "pch.h"

#include "theme_scripting.h"
#include "dro/system/animation.h"
#include "dro/interface/courtroom_layout.h"
#include "dro/interface/widgets/viewports/legacy_viewport.h"
#include "dro/system/replay_playback.h"

static int s_FrameRate = 60;
static int s_uptime = 0;
static double s_accumulatedTime = 0.0;
static RPTypewriter *s_typewriter = nullptr;
static RPViewport *s_viewport = nullptr;

static QElapsedTimer timer;
static bool s_loopPaused = false;

void RuntimeLoop::Update()
{
  static bool firstCall = true;

  if (firstCall) {
    timer.start();
    firstCall = false;
    return;
  }

  if(s_loopPaused) return;

  qint64 elapsedNano = timer.nsecsElapsed();
  timer.restart();

  int elapsedMillis = static_cast<int>(elapsedNano / 1e6);
  s_uptime += elapsedMillis;

  double deltaTime = elapsedNano / 1e9;
  LuaBridge::LuaEventCall("OnUpdate", deltaTime);

  s_accumulatedTime += deltaTime;

  const double targetDelta = 1.0 / s_FrameRate;

  dro::system::replays::playback::autoUpdate(s_uptime);

  dro::system::animation::runAll(elapsedMillis);

  if (s_accumulatedTime < targetDelta)
    return;

  s_accumulatedTime -= targetDelta;

  courtroom::viewport::update();

  if(s_viewport)
    s_viewport->update();

  if(s_typewriter)
    s_typewriter->update();

}

int RuntimeLoop::uptime()
{
  return s_uptime;
}

void RuntimeLoop::assignTypewriter(RPTypewriter *widget)
{
  s_typewriter = widget;
}

void RuntimeLoop::assignViewport(RPViewport *viewport)
{
  s_viewport = viewport;
}

void RuntimeLoop::setWindowFocus(bool focus)
{
  s_accumulatedTime = 0.0;
  if(focus) s_FrameRate = 60;
  else s_FrameRate = 15;
}

void RuntimeLoop::setPause(bool pauseState)
{
  s_loopPaused = pauseState;
  s_accumulatedTime = 0;
  timer.restart();
}
