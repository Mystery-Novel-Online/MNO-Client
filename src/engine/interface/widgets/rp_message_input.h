#ifndef RP_MESSAGE_INPUT_H
#define RP_MESSAGE_INPUT_H

#include <QTextEdit>
#include <QMap>
#include <QVector>
#include <QChar>
#include <QVariantList>
#include "engine/encoding/binary_encoding.h"
#include <qtextdocumentfragment.h>

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
  void clearTags();
  int maxLength();

protected:
  QMimeData * createMimeDataFromSelection() const override {
    QTextCursor cursor = textCursor();

    if (!cursor.hasSelection())
      return nullptr;

    QString output;

    int start = cursor.selectionStart();
    int end   = cursor.selectionEnd();

    QTextCursor walker(document());
    walker.setPosition(start);

    while (walker.position() < end)
    {
      walker.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
      QTextCharFormat fmt = walker.charFormat();

      if (fmt.isImageFormat())
      {
        QTextImageFormat imgFmt = fmt.toImageFormat();

        int targetTagId = imgFmt.property(QTextFormat::UserProperty).toInt();
        if(m_tags.contains(targetTagId))
        {
          MessageTag tag = m_tags[targetTagId];
          QVariantList tagArguments = engine::encoding::BinaryEncoder::decodeBase64(tag.value);

          QString tagType = "";
          switch((MessageTagType)tagArguments.at(0).toInt())
          {
            case TagType_Flip:
              output += "<flip>";
              break;

            case TagType_Hide:
              output += "<hide>";
              break;

            case TagType_NewLine:
              output += "<nl>";
              break;

            case TagType_Speed:
              output += "<speed:"+ QString::number(tagArguments.at(1).toInt()) +">";
              break;

            case TagType_Wait:
              output += "<wait:"+ QString::number(tagArguments.at(1).toInt()) +">";
              break;

            case TagType_MusicChange:
              output += "<music:"+ tagArguments.at(1).toString() +">";
              break;

            case TagType_SoundEffect:
              output += "<sfx:"+ tagArguments.at(1).toString() +">";
              break;

            case TagType_PlaySequence:
              output += "<anim:"+ tagArguments.at(1).toString() +">";
              break;

            case TagType_Color:
              output += "<highlight:"+ tagArguments.at(1).toString() +">";
              break;

            case TagType_Size:
              output += "<size:"+ QString::number(tagArguments.at(1).toDouble()) +">";
              break;

          default:
            break;
          }
        }

      } else {
        output += walker.selectedText();
      }

      walker.setPosition(walker.position());
    }

    auto *mime = new QMimeData();
    mime->setText(output);

    QByteArray orig;
    QDataStream stream(&orig, QIODevice::WriteOnly);
    cursor.selection().toHtml().toUtf8();
    mime->setData("application/x-rpmessage", output.toUtf8());

    return mime;
  }

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
