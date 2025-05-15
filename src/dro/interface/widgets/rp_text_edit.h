#ifndef RP_TEXT_EDIT_H
#define RP_TEXT_EDIT_H

#include <QTextEdit>

class AOApplication;
class RPTextEdit : public QTextEdit
{
  Q_OBJECT

  Q_PROPERTY(Qt::Alignment text_alignment READ get_text_alignment WRITE set_text_alignment NOTIFY text_alignment_changed)

public:
  RPTextEdit(const QString &name, QWidget *p_parent = nullptr);
  void resetTransform();

  Qt::Alignment get_text_alignment() const;

public slots:
  void set_outline(bool enabled, int outlineWidth = 1, QColor outlineColor = QColor(Qt::black));
  void set_auto_align(bool enabled);
  void set_text_alignment(Qt::Alignment alignment);
  void realign_text();

signals:
  void text_alignment_changed(Qt::Alignment);

private:
  AOApplication *m_app;
  Qt::Alignment m_text_align = Qt::AlignTop | Qt::AlignLeft;
  bool has_outline = false;
  bool is_auto_align = true;
  enum class Status
  {
    Done,
    InProgress,
  };
  Status m_status = Status::Done;
  int m_current_document_blocks = 0;
  int m_current_document_height = 0;

  void refresh_horizontal_alignment();
  void refresh_vertical_alignment();

  QString m_friendlyName = "";

private slots:
  void on_text_changed();
};

#endif // RP_TEXT_EDIT_H
