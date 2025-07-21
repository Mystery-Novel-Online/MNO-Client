#include "chat_log.h"

#include "aoconfig.h"

DRChatLog::DRChatLog(QWidget *parent)
    : QTextBrowser(parent)
    , dr_config(new AOConfig(this))
{
  connect(this, SIGNAL(message_queued()), this, SLOT(_p_write_message_queue()));
}

void DRChatLog::append_chatmessage(QString p_name, QString p_text)
{
  queue_message(p_name.trimmed().isEmpty() ? "Anonymous" : p_name, p_text, false);
}

void DRChatLog::append_information(QString p_text)
{
  queue_message(nullptr, p_text, false);
}

void DRChatLog::append_html(QString p_text)
{
  queue_message(nullptr, p_text, true);
}

void DRChatLog::reset_message_format()
{
  _p_reset_log();
}

void DRChatLog::queue_message(QString p_name, QString p_text, bool p_is_html)
{
  Message l_message;
  l_message.timestamp = QDateTime::currentDateTime();
  l_message.name = p_name;
  l_message.text = p_text;
  l_message.is_html = p_is_html;
  m_message_queue.append(std::move(l_message));
  Q_EMIT message_queued();
}

void DRChatLog::_p_write_message_queue()
{
  QScrollBar *l_scrollbar = verticalScrollBar();
  const bool l_is_end_scroll_pos = l_scrollbar->value() == l_scrollbar->maximum();

  QTextCharFormat l_normal_format;
  l_normal_format.setFont(font());
  l_normal_format.setFontWeight(QFont::Normal);
  QTextCharFormat l_name_format = l_normal_format;
  l_name_format.setFontWeight(QFont::Bold);
  QTextCharFormat l_href_format = l_normal_format;
  l_href_format.setAnchor(true);
  l_href_format.setFontWeight(QFont::Bold);
  l_href_format.setForeground(Qt::blue);
  l_href_format.setUnderlineStyle(QTextCharFormat::SingleUnderline);

  QTextCursor l_cursor = textCursor();
  l_cursor.movePosition(QTextCursor::End);
  l_cursor.setCharFormat(l_normal_format);

  while (!m_message_queue.isEmpty())
  {
    if (!m_message_list.isEmpty())
      l_cursor.insertText(QString(QChar::LineFeed));
    const Message l_message = m_message_queue.dequeue();
    m_message_list.append(l_message);

    l_cursor.insertText(QString(QString("[%1] ").arg(l_message.timestamp.toString("hh:mm"))), l_normal_format);

    if (!l_message.name.isEmpty())
    {
      l_cursor.insertText(l_message.name, l_name_format);
      l_cursor.insertText(": ", l_normal_format);
    }

    if (l_message.is_html)
    {
      insertHtml(l_message.text);
    }
    else
    {
      const QString l_text = l_message.text;

      class TextPiece
      {
      public:
        QString text;
        bool is_href = false;

        TextPiece()
        {}
        TextPiece(QString p_text, bool p_is_href = false)
            : text(p_text)
            , is_href(p_is_href)
        {}
      };
      QVector<TextPiece> l_piece_list;

      const QRegularExpression l_regex("(https?://[^\\s/$.?#].[^\\s]*)");
      QRegularExpressionMatchIterator l_iterator = l_regex.globalMatch(l_text);
      { // capture all text pieces
        int l_index = 0;

        while (l_iterator.hasNext())
        {
          QRegularExpressionMatch l_match = l_iterator.next();

          const int l_captureIndex = l_match.capturedStart();
          if (l_index < l_captureIndex)
            l_piece_list.append(l_text.mid(l_index, l_captureIndex - l_index));
          l_piece_list.append(TextPiece(l_match.captured(), true));
          l_index = l_match.capturedEnd();
        }

        l_piece_list.append(l_text.mid(l_index));
      }

      for (const TextPiece &i_piece : qAsConst(l_piece_list))
      {
        if (i_piece.text.isEmpty())
          continue;

        QTextCharFormat l_piece_format = l_normal_format;
        if (i_piece.is_href)
        {
          l_piece_format = l_href_format;
          l_piece_format.setAnchorHref(i_piece.text);
        }

        l_cursor.insertText(i_piece.is_href ? QUrl(i_piece.text).toString() : i_piece.text, l_piece_format);
      }
    }
  }

  if (l_is_end_scroll_pos)
    l_scrollbar->setValue(l_scrollbar->maximum());
}

void DRChatLog::_p_reset_log()
{
  clear();
  QQueue<Message> l_new_message_queue;
  dynamic_cast<QList<Message> &>(l_new_message_queue) = std::move(m_message_list);
  l_new_message_queue.append(std::move(m_message_queue));
  m_message_queue = std::move(l_new_message_queue);
  _p_write_message_queue();
}
