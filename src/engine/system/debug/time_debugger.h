#ifndef TIMEDEBUGGER_H
#define TIMEDEBUGGER_H

#include <QElapsedTimer>
#include <QString>
#include <QHash>



class TimeDebugger
{
public:
  TimeDebugger(const TimeDebugger&) = delete;

  static TimeDebugger& get()
  {
    return s_Instance;
  }

  void StartTimer(QString name);
  void CheckpointTimer(QString name, QString checkpoint);
  void EndTimer(QString name);

private:
  TimeDebugger() {}
  static TimeDebugger s_Instance;

  QHash<QString, QElapsedTimer> CurrentTimers = {};
  QHash<QString, qint64> mLastCheckpoint = {};
};

#endif // TIMEDEBUGGER_H
