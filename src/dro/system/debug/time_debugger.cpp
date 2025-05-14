#include "time_debugger.h"
#include "qdebug.h"

TimeDebugger TimeDebugger::s_Instance;

void TimeDebugger::StartTimer(QString name)
{
  if(!CurrentTimers.contains(name))
  {
    qDebug() << "[" + name + "] Starting timer.";
    QElapsedTimer timer;
    timer.start();
    CurrentTimers[name] = timer;
  }
}

void TimeDebugger::CheckpointTimer(QString name, QString checkpoint)
{
  if(CurrentTimers.contains(name))
  {
    qint64 Time = CurrentTimers[name].elapsed();
    QString message = "[" + QString::number(CurrentTimers[name].elapsed()) + "ms][" + checkpoint + "][" + name + "] Checkpoint!";
    if(mLastCheckpoint.contains(name)) message.append("(" + QString::number(Time - mLastCheckpoint[name]) + "ms since last checkpoint.)");
    qDebug() << message;
    mLastCheckpoint[name] = Time;
  }
}

void TimeDebugger::EndTimer(QString name)
{
  if(CurrentTimers.contains(name))
  {
    qDebug() << "[" + name + "] Timer ended with " + QString::number(CurrentTimers[name].elapsed()) + "ms";
    CurrentTimers.remove(name);
    if(mLastCheckpoint.contains(name)) mLastCheckpoint.remove(name);
  }

}
