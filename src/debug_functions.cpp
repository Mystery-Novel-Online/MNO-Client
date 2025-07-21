#include "debug_functions.h"
#include "pch.h"

#include "aoconfig.h"

enum class MessageType
{
  Notice,
  Warning,
  Prompt,
};

bool drMessageBox(QString p_message, MessageType p_message_type)
{
  AOConfig config;
  if (!config.display_notification(p_message))
    return QMessageBox::Accepted;

  QMessageBox message;
  const bool l_is_notice = p_message_type == MessageType::Notice;
  message.setWindowTitle(l_is_notice ? "Notice" : "Warning");
  message.setIcon(l_is_notice ? QMessageBox::Information : QMessageBox::Warning);
  message.setText(p_message);
  if (p_message_type == MessageType::Prompt)
  {
    message.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
  }
  QCheckBox *show_again = new QCheckBox(&message);
  show_again->setText(QObject::tr("&Show this message again"));
  show_again->setChecked(true);
  message.setCheckBox(show_again);
  message.exec();

  if (!show_again->isChecked())
    config.filter_notification(p_message);
  return message.result() == QMessageBox::Ok;
}

void call_notice(QString p_message)
{
  qInfo() << p_message;
  drMessageBox(p_message, MessageType::Notice);
}

void call_warning(QString p_message)
{
  qWarning() << "error:" << p_message;
  drMessageBox(p_message, MessageType::Warning);
}

bool prompt_warning(QString p_message)
{
  qDebug() << "prompt:" << p_message;
  return drMessageBox(p_message, MessageType::Prompt);
}
