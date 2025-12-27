#include "client_worker.h"
#include <QThread>

ClientWorker::ClientWorker(QObject *parent) : QObject(parent)
{}

void ClientWorker::process()
{
  m_running = true;

  for (int i = 0; i < 100 && m_running; ++i)
  {
    QThread::msleep(50);
    //Currently this worker runs 100 times every, pausing for 50ms.
    emit progress(i);
  }

  emit finished();
}

void ClientWorker::stop()
{
  m_running = false;
}
