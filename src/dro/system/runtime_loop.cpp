#include "runtime_loop.h"
#include <QElapsedTimer>
#include <qdebug.h>
#include "theme_scripting.h"
#include <QVector3D>
#include "dro/animation/keyframe_channel.h"
#include "dro/animation/keyframe_sequence.h"
#include "dro/system/animation.h"
#include "dro/interface/courtroom_layout.h"
#include "dro/interface/widgets/rp_typewriter.h"
#include "dro/interface/widgets/viewports/legacy_viewport.h"

static int s_FrameRate = 60;
static int s_uptime = 0;
static RPTypewriter *s_typewriter = nullptr;
static RPViewport *s_viewport = nullptr;

void RuntimeLoop::Update()
{
  static QElapsedTimer timer;
  static double accumulatedTime = 0.0;
  static bool firstCall = true;

  if (firstCall) {
    timer.start();
    firstCall = false;
    return;
  }

  qint64 elapsedNano = timer.nsecsElapsed();
  timer.restart();

  int elapsedMillis = static_cast<int>(elapsedNano / 1e6);
  s_uptime += elapsedMillis;

  double deltaTime = elapsedNano / 1e9;
  LuaBridge::LuaEventCall("OnUpdate", deltaTime);

  accumulatedTime += deltaTime;

  const double targetDelta = 1.0 / s_FrameRate;

  if (accumulatedTime < targetDelta)
    return;

  accumulatedTime -= targetDelta;

  dro::system::animation::runAll(elapsedMillis);

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
