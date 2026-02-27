#include "rp_message_input.h"
#include "engine/encoding/binary_encoding.h"

RPMessageInput::RPMessageInput(QWidget *parent)
    : QTextEdit(parent)
{
  setAcceptRichText(true);
  setTextInteractionFlags(Qt::TextEditorInteraction);
  setLineWrapMode(QTextEdit::NoWrap);
  setWordWrapMode(QTextOption::NoWrap);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFrameStyle(QFrame::NoFrame);

  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  document()->setDocumentMargin(0);
  setFixedHeight(fontMetrics().lineSpacing() + 8);

  connect(this, &QTextEdit::textChanged, this, &RPMessageInput::handleTextChanged);
  reloadHighlights();

  setTabChangesFocus(true);
}

void RPMessageInput::reloadHighlights()
{
  m_highlights.clear();
  m_openingCharacters.clear();

  QVector<QStringList> rawHighlights = AOApplication::getInstance()->get_highlight_colors();
  for (const QStringList& h : rawHighlights)
  {
    if (h.size() >= 3)
    {
      m_highlights.append({ h[2] == "0", h[0][0], h[0][1], h[1] });
      m_openingCharacters.append(h[0][0]);
    }
  }

  m_highlightQueue.clear();
}

void RPMessageInput::setMaxLength(int length)
{
  if(length < 0)
    length = 0;
  m_maxLength = length;
}

void RPMessageInput::addTag(MessageTagType type, QVariantList arguments)
{
  QTextCursor cursor = textCursor();
  QTextImageFormat format = createTagFormat(type);
  int id = format.property(QTextFormat::UserProperty).toInt();
  cursor.insertImage(format);

  MessageTag tag;
  tag.id = id;
  arguments.prepend(type);
  tag.value = engine::encoding::BinaryEncoder::encodeBase64(arguments);
  m_tags[id] = tag;
}

QString RPMessageInput::text() const
{
  QString result;
  QTextBlock block = document()->begin();

  while (block.isValid())
  {
    QTextBlock::iterator it;
    for (it = block.begin(); !(it.atEnd()); ++it)
    {
      QTextFragment frag = it.fragment();
      if (frag.isValid() && !frag.charFormat().isImageFormat())
      {
        result += frag.text();
      }
    }
    block = block.next();
  }

  return result;
}

QVector<MessageTag> RPMessageInput::getTags()
{
  QVector<MessageTag> tags;

  QTextBlock block = document()->begin();
  while (block.isValid())
  {
    QTextBlock::iterator it = block.begin();
    while (!it.atEnd())
    {
      QTextFragment frag = it.fragment();
      if (!frag.isValid()) {
        ++it;
        continue;
      }

      const QTextCharFormat format = frag.charFormat();

      if (format.isImageFormat())
      {
        QTextImageFormat imageFormat = format.toImageFormat();

        int targetTagId = imageFormat.property(QTextFormat::UserProperty).toInt();
        if(m_tags.contains(targetTagId))
        {
          MessageTag tag = m_tags[targetTagId];
          QVariantList tagArguments = engine::encoding::BinaryEncoder::decodeBase64(tag.value);
          if((MessageTagType)tagArguments.at(0).toInt() == TagType_Wait)
          {
            tag.index = (frag.position() - 1 - (tags.count()));
          }
          else
            tag.index = (frag.position() - (tags.count()));
          tag.id = targetTagId;
          tags.append(tag);
        }
      }

      ++it;
    }

    block = block.next();
  }

  return tags;
}

void RPMessageInput::clearTags()
{
  m_tags.clear();
}

int RPMessageInput::maxLength()
{
  return m_maxLength;
}

QTextImageFormat RPMessageInput::createTagFormat(MessageTagType tagType)
{
  QTextImageFormat format;
  switch (tagType) {
  case MessageTagType::TagType_MusicChange:
    format.setName("base/tags/change_music.png");
    break;
  case MessageTagType::TagType_Flip:
    format.setName("base/tags/flip.png");
    break;
  case MessageTagType::TagType_PlaySequence:
    format.setName("base/tags/animation.png");
    break;
  default:
    format.setName("base/tags/default.png");
    break;
  }
  int h = fontMetrics().height() - 2;

  format.setWidth(h);
  format.setHeight(h);

  int id = 1040 + m_tags.count();
  format.setProperty(QTextFormat::UserProperty, QVariant::fromValue(id));
  return format;
}

void RPMessageInput::applyHighlighting(const QString &text)
{
  QTextCharFormat normalFormat;
  QColor textColor = this->palette().color(QPalette::Text);
  normalFormat.setForeground(textColor);

  QList<QTextCharFormat> formatStack;
  m_highlightQueue.clear();
  bool ignoreNext = false;

  for (int i = 0; i < text.length(); ++i) {
    QChar c = text[i];
    QTextCursor charCursor(document());
    charCursor.setPosition(i);
    charCursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);

    if (charCursor.charFormat().isImageFormat())
      continue;

    if (!ignoreNext && c == '\\') {
      ignoreNext = true;
      charCursor.setCharFormat(normalFormat);
      continue;
    }

    if (!ignoreNext) {
      if (closeHighlight(c, charCursor, formatStack)) continue;
      if (openHighlight(c, charCursor, formatStack)) continue;
    }

    QTextCharFormat formatToApply = formatStack.isEmpty() ? normalFormat : formatStack.last();
    charCursor.setCharFormat(formatToApply);
    ignoreNext = false;
  }
}

bool RPMessageInput::closeHighlight(QChar c, QTextCursor &cursor, QList<QTextCharFormat> &formatStack)
{
  if (m_highlightQueue.isEmpty()) return false;

  const ThemeMessageHighlight& last = m_highlightQueue.last();
  if (last.closingCharacter != c) return false;

  QTextCharFormat format;
  format.setForeground(last.hideCharacter ? QColor(last.colourValue + "80") : QColor(last.colourValue));

  cursor.setCharFormat(format);
  formatStack.pop_back();
  m_highlightQueue.removeLast();
  return true;
}

bool RPMessageInput::openHighlight(QChar c, QTextCursor &cursor, QList<QTextCharFormat> &formatStack)
{
  for (const auto& h : m_highlights)
  {
    if (h.openingCharacter == c)
    {
      m_highlightQueue.append(h);

      QTextCharFormat markerFormat;
      markerFormat.setForeground(h.hideCharacter ? QColor(h.colourValue + "80") : QColor(h.colourValue));
      cursor.setCharFormat(markerFormat);

      QTextCharFormat highlightFormat;
      highlightFormat.setForeground(QColor(h.colourValue));
      formatStack.push_back(highlightFormat);
      return true;
    }
  }
  return false;
}

void RPMessageInput::handleTextChanged()
{
  static QString lastText = "";
  QSignalBlocker blocker(this);

  QString plain = toPlainText();
  QTextCursor cursor = textCursor();
  int savedPos = cursor.position();

  if (plain.length() > m_maxLength) {
    plain = plain.left(m_maxLength);

    setPlainText(plain);

    if (savedPos > plain.length())
      savedPos = plain.length();
    cursor.setPosition(savedPos);
    setTextCursor(cursor);
    return;
  }
  lastText = plain;

  savedPos = textCursor().position();
  cursor = QTextCursor(document());
  cursor.beginEditBlock();

  int i = 0;
  while (i < plain.length()) {
    if(plain[i] == '\n')
    {
      QTextCursor removeCursor(document());
      removeCursor.setPosition(i + 1);
      removeCursor.setPosition(i, QTextCursor::KeepAnchor);
      removeCursor.removeSelectedText();
      plain = toPlainText();
      i++;
      continue;
    }
    if (plain[i] == '<') {
      int end = plain.indexOf('>', i);
      if (end == -1)
        break;

      QString tagText = plain.mid(i + 1, end - i - 1);
      QStringList parts = tagText.split(':');
      QString tagTypeStr = parts[0];
      parts.removeFirst();

      QVariantList args;
      bool removeData = true;
      MessageTagType tagType;

      if (tagTypeStr == "flip" && parts.isEmpty()) tagType = TagType_Flip;
      else if (tagTypeStr == "hide" && parts.isEmpty()) tagType = TagType_Hide;
      else if (tagTypeStr == "nl" && parts.isEmpty()) tagType = TagType_NewLine;
      else if (tagTypeStr == "wait" && parts.count() == 1) { args.append(parts[0].toInt()); tagType = TagType_Wait; }
      else if (tagTypeStr == "speed" && parts.count() == 1) { args.append(parts[0].toInt()); tagType = TagType_Speed; }
      else if (tagTypeStr == "music" && parts.count() == 1) { args.append(parts[0]); tagType = TagType_MusicChange; }
      else if (tagTypeStr == "sfx" && parts.count() == 1) { args.append(parts[0]); tagType = TagType_SoundEffect; }
      else if (tagTypeStr == "anim" && parts.count() == 1) { args.append(parts[0]); tagType = TagType_PlaySequence; }
      else if (tagTypeStr == "highlight" && parts.count() == 1) { args.append(parts[0]); tagType = TagType_Color; }
      else if (tagTypeStr == "scale" && parts.count() == 1) { args.append(parts[0].toDouble()); tagType = TagType_Size; }
      else removeData = false;

      if (removeData) {
        QTextCursor insertCursor(document());
        insertCursor.setPosition(i);
        QTextImageFormat format = createTagFormat(tagType);

        int id = format.property(QTextFormat::UserProperty).toInt();
        insertCursor.insertImage(format);

        MessageTag tag;
        tag.id = id;
        args.prepend(tagType);
        tag.value = engine::encoding::BinaryEncoder::encodeBase64(args);
        m_tags[id] = tag;

        QTextCursor removeCursor(document());
        removeCursor.setPosition(i + 1);
        removeCursor.setPosition(i + (end - i + 1) + 1, QTextCursor::KeepAnchor);
        removeCursor.removeSelectedText();

        plain = toPlainText();
        i++;
        continue;
      }

      i = end + 1;
      continue;
    }
    i++;
  }

  applyHighlighting(plain);

  cursor.endEditBlock();

  QTextCursor finalCursor = textCursor();
  finalCursor.setPosition(qMin(savedPos, finalCursor.document()->characterCount() - 1));
  setTextCursor(finalCursor);
}

void RPMessageInput::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
  {
    emit returnPressed();
    e->accept();
  }
  else if (e->key() == Qt::Key_Tab)
  {
    e->ignore();
    return;
  }
  else
    QTextEdit::keyPressEvent(e);
}
