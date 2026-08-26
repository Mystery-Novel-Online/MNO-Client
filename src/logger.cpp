#include "logger.h"

#include <atomic>
#include <thread>

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMutex>
#include <QSystemSemaphore>
#include <QTextStream>
#include <QThread>

// Get the default Qt message handler.
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(0);
static const int C_MAX_FILE_SIZE = 1e7; // 10 MB
static const QString C_FILE_NAME = "base/logs/debug.log";
static const QString C_FILE_NAME_BACKUP = "base/logs/debug_b.log";
static bool s_verbose_logging = false;

static bool s_initialized = false;
static QMutex s_msg_list_lock;
static QStringList s_msg_list;
static std::thread s_logger;
static std::atomic_bool s_continue = false;

void check_file_size_and_rename()
{
  QFile l_log(C_FILE_NAME);
  if(l_log.exists() && l_log.size() >= C_MAX_FILE_SIZE)
  {
    if(l_log.exists(C_FILE_NAME_BACKUP))
    {
      if(!l_log.remove(C_FILE_NAME_BACKUP))
      {
        s_continue = false;
        qInstallMessageHandler(0);
        qWarning() << "error: failed to remove log file" << l_log.errorString();
        return;
      }
    }

    if(!l_log.rename(C_FILE_NAME_BACKUP))
    {
      s_continue = false;
      qInstallMessageHandler(0);
      qWarning() << "error: failed to rename log file" << l_log.errorString();
    }
  }
}

void task()
{
  s_continue = true;
  QSystemSemaphore l_system_lock("dro-logger-lock", 1);
  QStringList l_msg_list;
  while(s_continue)
  {
    if(s_msg_list_lock.tryLock())
    {
      QStringList l_msg_list(std::move(s_msg_list));
      s_msg_list_lock.unlock();

      if(!l_msg_list.isEmpty())
      {
        l_system_lock.acquire();
        check_file_size_and_rename();
        QFile l_log(C_FILE_NAME);
        if(l_log.open(QFile::WriteOnly | QFile::Append))
        {
          QTextStream in(&l_log);
          for(const QString &i_msg : l_msg_list)
          {
            in << i_msg
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
               << Qt::endl;
#else
               << "\n";
#endif
          }
        }
        l_log.close();
        l_system_lock.release();
      }
    }

    QThread::msleep(10);
  }
}

QString generate_message(QtMsgType p_type, const QMessageLogContext &p_context, const QString &p_message)
{
  QString l_output;
  switch (p_type)
  {
  case QtDebugMsg:
    l_output = QString("D");
    break;
  case QtInfoMsg:
    l_output = QString("I");
    break;
  case QtWarningMsg:
    l_output = QString("W");
    break;
  case QtCriticalMsg:
    l_output = QString("C");
    break;
  case QtFatalMsg:
    l_output = QString("F");
    break;
  }

  const QString l_timestamp = QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz");
  const QString l_pid = QString::number(QCoreApplication::applicationPid());

  QString l_final_message = p_message;
  // Censor HDID
  if(p_message.startsWith("S/S: HI#"))
    l_final_message = "S/S: HI#HDID#%";
  else if(p_message.startsWith("M/S: HI#"))
    l_final_message = "M/S: HI#HDID#%";
  else
    l_final_message = p_message;

  l_output = QString("[%1][%2][%3] %4").arg(l_timestamp, l_pid, l_output, l_final_message);
  if(s_verbose_logging)
    l_output.append(QString(" (%1:%2, %3)").arg(p_context.file).arg(p_context.line).arg(p_context.function));

  return l_output;
}

void save_log_line(QString p_log_line)
{
  s_msg_list_lock.lock();
  s_msg_list.append(p_log_line);
  s_msg_list_lock.unlock();
}

void logger::log(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
  if(!s_initialized)
  {
    s_initialized = true;
    s_logger = std::thread(task);
  }

  save_log_line(generate_message(type, context, msg));

  // call the default handler.
  (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
}

void logger::shutdown()
{
  s_continue = false;
  try
  {
    if(s_logger.joinable())
      s_logger.join();
  }
  catch (const std::exception &e)
  {
    qWarning() << "error:" << e.what();
  }
  catch (...)
  {
    qWarning() << "error: unknown exception";
  }
}

void logger::set_verbose_logging(bool enabled)
{
  s_verbose_logging = enabled;
}
