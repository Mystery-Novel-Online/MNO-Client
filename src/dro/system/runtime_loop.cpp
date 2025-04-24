#include "runtime_loop.h"
#include <QElapsedTimer>
#include <qdebug.h>
#include "theme_scripting.h"

void RuntimeLoop::Update()
{
  static QElapsedTimer timer;
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
}
