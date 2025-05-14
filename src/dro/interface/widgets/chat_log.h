#pragma once

class AOConfig;

#include <QDateTime>
#include <QList>
#include <QQueue>
#include <QTextBrowser>

class DRChatLog : public QTextBrowser
{
  Q_OBJECT

public:
  DRChatLog(QWidget *parent = nullptr);

  void append_chatmessage(QString name, QString message);
  void append_information(QString message);
  void append_html(QString html);
  void reset_message_format();

signals:
  void message_queued();

private:
  AOConfig *dr_config = nullptr;

  class Message
  {
  public:
    QDateTime timestamp;
    QString name;
    QString text;
    bool is_html = false;
  };
  QList<Message> m_message_list;
  QQueue<Message> m_message_queue;

  void queue_message(QString name, QString text, bool is_html);

private slots:
  void _p_write_message_queue();
  void _p_reset_log();
};
