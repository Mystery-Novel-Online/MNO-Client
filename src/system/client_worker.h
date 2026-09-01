#ifndef CLIENT_WORKER_H
#define CLIENT_WORKER_H
#include <QObject>

class ClientWorker : public QObject
{
  Q_OBJECT
public:
  explicit ClientWorker(QObject *parent = nullptr);

public slots:
  void process();
  void stop();

signals:
  void progress(int value);
  void finished();

private:
  bool m_running = true;
};

#endif // CLIENT_WORKER_H
