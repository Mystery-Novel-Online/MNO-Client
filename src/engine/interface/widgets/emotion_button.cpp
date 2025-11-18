#include "engine/param/actor/actor_loader.h"
#include "modules/theme/thememanager.h"
#include "engine/interface/menus/emote_menu.h"
#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_characters.h"
#include "engine/param/actor_repository.h"

using namespace engine::actor::user;
AOEmoteButton::AOEmoteButton(QWidget *p_parent, AOApplication *p_ao_app, int p_x, int p_y)
    : QPushButton(p_parent)
{
  ao_app = p_ao_app;

  this->move(p_x, p_y);

  float buttonSize = EmoteMenu::isDoubleSize() ? 82 : 40;

  int resizedButtonSize = (int)(buttonSize * ThemeManager::get().getResize());

  this->resize(resizedButtonSize, resizedButtonSize);

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

void AOEmoteButton::setLayerImage(const QString &character, const QString &layer, const QString &outfit, bool enabled)
{
  rolechat::actor::IActorData *actor = retrieve();

  QString l_texture = "";
  ui_selected->hide();

  if (enabled)
  {
    const QString l_selected_texture = "THEME TOGGLE TEXTURE HERE";

    if (FS::Checks::FileExists(l_selected_texture))
    {
      ui_selected->setStyleSheet(QString("border-image: url(\"%1\")").arg(l_selected_texture));
      ui_selected->show();
    }
    else
    {
        ui_selected->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, "
                                   "y2:1, stop:0 rgba(0, 0, 0, 0), stop:1 rgba(0, 0, 0, 127)); }");
        ui_selected->show();
    }

  }


  m_texture.load(l_texture);
  m_comment = layer;
  setText(m_texture.isNull() ? layer : nullptr);
}

int AOEmoteButton::get_emote_number()
{
  return m_index;
}

void AOEmoteButton::set_image(ActorEmote p_emote, bool p_enabled)
{
  rolechat::actor::IActorData *actor = retrieve();

  QString l_texture = QString::fromStdString(actor->buttonImage(p_emote, false));
  l_texture = fs::characters::getFilePath(QString::fromStdString(p_emote.character), l_texture);

  // reset states
  ui_selected->hide();

  // nested ifs are okay
  if (p_enabled)
  {
    const QString l_selected_texture = fs::characters::getFilePath(QString::fromStdString(p_emote.character), QString::fromStdString(actor->selectedImage(p_emote)));

    if (FS::Checks::FileExists(l_selected_texture))
    {
      ui_selected->setStyleSheet(QString("border-image: url(\"%1\")").arg(l_selected_texture));
      ui_selected->show();
    }
    else
    {
      const QString l_enabled_texture = fs::characters::getFilePath(QString::fromStdString(p_emote.character), QString::fromStdString(actor->buttonImage(p_emote, true)));;

      if (FS::Checks::FileExists(l_enabled_texture))
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

  if(EmoteMenu::isRealtime())
  {
    m_texture.load(fs::characters::getSpritePath(p_emote.character, p_emote.dialog, "", false));
    m_texture = m_texture.scaledToHeight(250, Qt::SmoothTransformation);

    int highestPixel = findHighestPixel();

    int cropY = 0;
    if (highestPixel != -1) cropY = highestPixel + 30;

    if (cropY + 82 > m_texture.height()) cropY = m_texture.height() - 82;
    if (cropY < 0) cropY = 0;

    int cropX = (m_texture.width() - 82) / 2;

    m_texture = m_texture.copy(cropX, cropY, 82, 82);
  }
  else
  {
    m_texture.load(l_texture);
  }
  m_comment = QString::fromStdString(p_emote.comment);
  setText(m_texture.isNull() ? QString::fromStdString(p_emote.comment) : nullptr);
}

int AOEmoteButton::findHighestPixel()
{
  for (int y = 0; y < m_texture.height(); ++y)
  {
    const QRgb* scanLine = reinterpret_cast<const QRgb*>(m_texture.scanLine(y));
    for (int x = 0; x < m_texture.width(); ++x)
    {
      if (qAlpha(scanLine[x]) > 0) return y;
    }
  }

  return -1;

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

