#include "aoemotebutton.h"

#include "aoapplication.h"
#include "courtroom.h"
#include "file_functions.h"
#include "modules/theme/thememanager.h"
#include <QHelpEvent>
#include <QImage>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <modules/managers/character_manager.h>

AOEmoteButton::AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y)
    : QPushButton(p_parent)
{
  ao_app = p_ao_app;

  this->move(p_x, p_y);
  this->resize((int)((float)40 * ThemeManager::get().getResize()), (int)((float)40 * ThemeManager::get().getResize()));

  ui_selected = new QLabel(this);
  ui_selected->resize(size());
  ui_selected->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_selected->hide();

  connect(this, &QAbstractButton::clicked, this, &AOEmoteButton::on_clicked);
}

void AOEmoteButton::set_emote_number(int p_emote_number)
{
  m_index = p_emote_number;
}

int AOEmoteButton::get_emote_number()
{
  return m_index;
}

void AOEmoteButton::set_image(DREmote p_emote, bool p_enabled)
{
  QString l_texture = CharacterManager::get().p_SelectedCharacter->getEmoteButton(p_emote, false);

  // reset states
  ui_selected->hide();

  // nested ifs are okay
  if (p_enabled)
  {

    const QString l_selected_texture = CharacterManager::get().p_SelectedCharacter->getSelectedImage(p_emote);



    if (file_exists(l_selected_texture))
    {
      ui_selected->setStyleSheet(QString("border-image: url(\"%1\")").arg(l_selected_texture));
      ui_selected->show();
    }
    else
    {
      const QString l_enabled_texture = CharacterManager::get().p_SelectedCharacter->getEmoteButton(p_emote, true);

      if (file_exists(l_enabled_texture))
      {
        l_texture = l_enabled_texture;
      }
      else
      {
        ui_selected->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, "
                                   "y2:1, stop:0 rgba(0, 0, 0, 0), stop:1 rgba(0, 0, 0, 127)); }");
        ui_selected->show();
      }
    }

  }

  m_texture.load(l_texture);
  m_comment = p_emote.comment;
  setText(m_texture.isNull() ? p_emote.comment : nullptr);
}

void AOEmoteButton::on_clicked()
{
  Q_EMIT emote_clicked(m_index);
}

void AOEmoteButton::paintEvent(QPaintEvent *event)
{
  if (m_texture.isNull())
  {
    QPushButton::paintEvent(event);
    return;
  }

  QPainter l_painter(this);
  l_painter.drawImage(rect(), m_texture.scaled(size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
  l_painter.end();
}

bool AOEmoteButton::event(QEvent *event)
{
  switch (event->type())
  {
  case QEvent::ToolTip:
    Q_EMIT tooltip_requested(m_index, dynamic_cast<QHelpEvent *>(event)->globalPos());
    break;

  case QEvent::Leave:
    Q_EMIT mouse_left(m_index);
    break;

  default:
    break;
  }

  return QPushButton::event(event);
}

