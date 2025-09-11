#ifndef RP_MESSAGE_INPUT_H
#define RP_MESSAGE_INPUT_H

#include <QTextEdit>
#include <QMap>
#include <QVector>
#include <QChar>
#include <QVariantList>

class QTextCursor;
class QTextCharFormat;
class QTextImageFormat;
class QKeyEvent;



class RPMessageInput : public QTextEdit
{
  Q_OBJECT

public:
  RPMessageInput(QWidget *parent = nullptr);

  // Public API
  void reloadHighlights();
  void setMaxLength(int length);
  void addTag(MessageTagType type, QVariantList arguments);

  QString text() const;
  QVector<MessageTag> getTags();
  int maxLength();

signals:
  void returnPressed();

protected:
  void keyPressEvent(QKeyEvent *e) override;

private slots:
  void handleTextChanged();

private:
  // Tag helpers
  QTextImageFormat createTagFormat(MessageTagType tagType);

  // Highlighting logic
  void applyHighlighting(const QString& text);
  bool closeHighlight(QChar c, QTextCursor& cursor, QList<QTextCharFormat>& formatStack);
  bool openHighlight(QChar c, QTextCursor& cursor, QList<QTextCharFormat>& formatStack);

  // Tag + highlight data
  QMap<int, MessageTag> m_tags = {};
  QVector<ThemeMessageHighlight> m_highlights;
  QVector<ThemeMessageHighlight> m_highlightQueue = {};
  QVector<QChar> m_openingCharacters = {};

  int m_maxLength = 256;
};


#endif // RP_MESSAGE_INPUT_H
