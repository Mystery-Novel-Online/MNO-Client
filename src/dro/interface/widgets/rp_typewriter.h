#ifndef RP_TYPEWRITER_H
#define RP_TYPEWRITER_H

#include <QObject>
#include "dro/interface/widgets/rp_text_edit.h"

enum TypeWriterAction
{
  TypeWriterAction_Text,
  TypeWriterAction_Shake,
  TypeWriterAction_SpeedUp,
  TypeWriterAction_SpeedDown,
  TypeWriterAction_NewLine,
  TypeWriterAction_Append
};

class RPTypewriter : public RPTextEdit
{
  Q_OBJECT
public:
  RPTypewriter(QWidget *p_parent = nullptr);
  ~RPTypewriter();

  void setInput(QString inputText);
  void update();
  bool typingComplete();

private:
  int m_currentIndex = 0;

  QString m_inputText = "";
  QString m_renderedText = "";

  int m_lastUpdate = 0;

  int m_tickInterval = 30;

  QChar m_currentColor = ' ';
  QVector<QString> m_queuedHighlights = {};
  QMap<int, TypeWriterAction> m_QueuedActions = {};
};

#endif // RP_TYPEWRITER_H
