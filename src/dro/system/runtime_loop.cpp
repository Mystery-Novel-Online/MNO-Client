#include "runtime_loop.h"
#include <QElapsedTimer>
#include <qdebug.h>
#include "theme_scripting.h"
#include <QVector3D>
#include "dro/animation/keyframe_channel.h"
#include "dro/animation/keyframe_sequence.h"
#include "dro/system/animation.h"
#include "dro/interface/courtroom_layout.h"

static int s_FrameRate = 60;

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


  double deltaTime = elapsedNano / 1e9;
  LuaBridge::LuaEventCall("OnUpdate", deltaTime);

  accumulatedTime += deltaTime;

  const double targetDelta = 1.0 / s_FrameRate;

  if (accumulatedTime < targetDelta)
    return;

  accumulatedTime -= targetDelta;

  dro::system::animation::runAll(elapsedNano / 1e6);
  courtroom::viewport::update();

}
