#include "rp_text_edit.h"
#include "debug_functions.h"

#include "theme.h"
#include "engine/system/theme.h"

RPTextEdit::RPTextEdit(const QString &name, QWidget *parent)
    : QTextEdit(parent), m_friendlyName(name)
{
  m_app = AOApplication::getInstance();
  connect(this, SIGNAL(textChanged()), this, SLOT(on_text_changed()));
  connect(this, SIGNAL(text_alignment_changed(Qt::Alignment)), this, SLOT(on_text_changed()));
}

void RPTextEdit::resetTransform()
{
  engine::system::theme::applyDimensions(this, m_friendlyName, ThemeSceneType::SceneType_Courtroom);
}

void RPTextEdit::hideScrollbars()
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void RPTextEdit::set_outline(bool p_enabled, int outlineWidth, QColor outlineColor)
{
  if (has_outline == p_enabled)
    return;
  has_outline = p_enabled;
  QTextCharFormat widget_format = currentCharFormat();
  if (p_enabled)
    widget_format.setTextOutline(QPen(outlineColor, outlineWidth));
  else
    widget_format.setTextOutline(Qt::NoPen);
  setCurrentCharFormat(widget_format);
}

void RPTextEdit::set_auto_align(bool p_enabled)
{
  if (is_auto_align == p_enabled)
    return;
  is_auto_align = p_enabled;
}

void RPTextEdit::set_text_alignment(Qt::Alignment p_align)
{
  if (m_text_align == p_align)
    return;
  m_text_align = p_align;
  Q_EMIT text_alignment_changed(m_text_align);
}

void RPTextEdit::realign_text()
{
  if (m_status == Status::InProgress)
    return;
  m_status = Status::InProgress;
  setAlignment(m_text_align);
  //  refresh_horizontal_alignment();
  refresh_vertical_alignment();
  m_status = Status::Done;
}

Qt::Alignment RPTextEdit::get_text_alignment() const
{
  return m_text_align;
}

void RPTextEdit::on_text_changed()
{
  if (!is_auto_align)
    return;
  realign_text();
}

void RPTextEdit::refresh_horizontal_alignment()
{
  // This stores the number of blocks. We use blocks here as Qt makes its horizontal
  // alignment based on blocks (rather than apparent line breaks/places where words wrap).
  int new_document_blocks = document()->blockCount();
  if (document()->toPlainText().isEmpty())
  {
    // Qt is very special and does not set this to 0 for empty documents.
    m_current_document_blocks = 0;
    // We also don't need to do any adjusting for empty documents, so return immediately
    return;
  }
  // If we have not changed the number of blocks in the document with this new incoming text change,
  // We do not need to update anything, so we exit early.
  if (new_document_blocks == m_current_document_blocks)
    return;

  // Otherwise, we have changed the number of blocks. By induction only the current block needs to be
  // updated, which is why we can get away with doing this setAlignment once, and right here.
  // qDebug() << this << document()->toPlainText() << new_document_blocks;
  setAlignment(m_text_align);
}

void RPTextEdit::refresh_vertical_alignment()
{
  // This stores the total height of the totality of the text saved.
  int new_document_height = document()->size().height();
  if (document()->toPlainText().isEmpty())
  {
    // Qt is very special and does not set this to 0 for empty documents.
    m_current_document_height = 0;
    // We also don't need to do any adjusting for empty documents, so return immediately
    return;
  }
  // If we have not changed the document height with this new incoming text change,
  // We do not need to update anything, so we exit early.
  if (new_document_height == m_current_document_height)
    return;

  m_current_document_height = new_document_height;

  // The way we will simulate vertical alignment is by adjusting the top margin to simulate
  // center alignment, or bottom alignment.
  int top_margin = 0;
  switch (m_text_align & (Qt::AlignVCenter | Qt::AlignBottom))
  {
  case Qt::AlignVCenter:
    top_margin = (height() - new_document_height) / 2;
    break;
  case Qt::AlignBottom:
    top_margin = (height() - new_document_height);
    break;
  default:
    break;
  }
  setViewportMargins(0, top_margin, 0, 0);
}
