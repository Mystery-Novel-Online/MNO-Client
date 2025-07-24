#include "rp_typewriter.h"
#include "engine/system/runtime_loop.h"
#include "engine/system/audio.h"

RPTypewriter::RPTypewriter(QWidget *parent) : RPTextEdit("message", parent)
{
  audio::blip::SetGender("male");
  RuntimeLoop::assignTypewriter(this);
}

RPTypewriter::~RPTypewriter()
{
  RuntimeLoop::assignTypewriter(nullptr);
}

void RPTypewriter::setInput(QString inputText)
{
  setText("");
  m_lastUpdate = 0;
  m_inputText = "";
  m_renderedText = "";
  m_currentColor = ' ';
  m_currentIndex = 0;
  m_queuedHighlights.clear();
  if(inputText.trimmed().isEmpty()) return;
  bool ignoreNextCharacter = false;

  for(const QChar& letter : inputText)
  {
    if (!ignoreNextCharacter && letter == Qt::Key_Backslash)
    {
      ignoreNextCharacter = true;
      continue;
    }
    else if(!ignoreNextCharacter && (letter == Qt::Key_BraceLeft || letter == Qt::Key_BraceRight))
    {
      m_QueuedActions[m_inputText.count()] = (letter == Qt::Key_BraceRight) ? TypeWriterAction_SpeedUp : TypeWriterAction_SpeedDown;
      continue;
    }
    else if(ignoreNextCharacter && letter == 's')
    {
      ignoreNextCharacter = false;
      m_QueuedActions[m_inputText.count()] = TypeWriterAction_Shake;
    }
    else
    {
      m_inputText.append(letter);
    }
    ignoreNextCharacter = false;
  }
  update();
}

void RPTypewriter::update()
{
  int currentUptime = RuntimeLoop::uptime();
  if((currentUptime - m_lastUpdate) < m_tickInterval)
    return;

  if(m_currentIndex >= m_inputText.count()) return;
  if(m_currentIndex == 0) audio::blip::Tick();

  QTextCharFormat charFormat = currentCharFormat();
  QChar incomingChar = m_inputText.at(m_currentIndex);
  QVector<QStringList> messageHighlights = AOApplication::getInstance()->get_highlight_colors();

  QColor textColor;
  if(m_queuedHighlights.count() == 0)
    textColor.setNamedColor("#F9FFFE");
  else
    textColor.setNamedColor(m_queuedHighlights.at(m_queuedHighlights.count() - 1));
  charFormat.setForeground(textColor);

  for(QStringList mMessageHighlights : messageHighlights)
  {
    bool existingColor = false;
    if(m_queuedHighlights.count() != 0)
    {
      if(mMessageHighlights[1] == m_queuedHighlights.at(m_queuedHighlights.count() - 1))
      {
        if(incomingChar == mMessageHighlights[0].at(1))
        {
          existingColor = true;
          m_currentColor = ' ';
          m_queuedHighlights.removeLast();
          if(mMessageHighlights[2] == "0")
          {
            m_inputText.remove(m_currentIndex, 1);
            return;
          }
        }
      }
    }

    if(mMessageHighlights[0].at(0) == incomingChar && !existingColor)
    {
      QColor text_color;
      text_color.setNamedColor(mMessageHighlights[1]);
      charFormat.setForeground(text_color);
      m_currentColor = incomingChar;
      m_queuedHighlights.append(mMessageHighlights[1]);
      if(mMessageHighlights[2] == "0")
      {
        textCursor().insertText("", charFormat);
        m_inputText.remove(m_currentIndex, 1);
        return;
      }
    }
  }

  textCursor().insertText(incomingChar, charFormat);
  m_renderedText.append(incomingChar);

  m_currentIndex += 1;
  m_lastUpdate = currentUptime;

  if(typingComplete())
  {
    emit typingDone();
  }
}

bool RPTypewriter::typingComplete()
{
  return m_renderedText == m_inputText;
}
