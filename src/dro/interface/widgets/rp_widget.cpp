#include "rp_widget.h"
#include <QMouseEvent>
#include "aoapplication.h"
#include "commondefs.h"
#include <modules/theme/thememanager.h>
#include "dro/fs/fs_reading.h"

RPWidget::RPWidget(QWidget *parent) : QWidget{parent}
{
  m_App = AOApplication::getInstance();
}

void RPWidget::setDragable(bool isDragable)
{
  m_dragEnabled = isDragable;
}

void RPWidget::setBackgroundImage(QString imageName)
{
  if(m_backgroundImage == nullptr) m_backgroundImage = new DRStickerViewer(m_App, this);
  m_backgroundImage->move(0, 0);
  m_backgroundImage->resize(width(), height());

  if (m_App->find_theme_asset_path(imageName, FS::Formats::SupportedImages()).isEmpty())
  {
    m_backgroundImage->stop();
    m_backgroundImage->hide();
    return;
  }

  m_backgroundImage->set_theme_image(imageName);
  m_backgroundImage->show();
  m_backgroundImage->lower();
  set_sticker_play_once(m_backgroundImage, imageName, COURTROOM_CONFIG_INI, m_App);
}

void RPWidget::mousePressEvent(QMouseEvent *event)
{
  if(!m_dragEnabled) return;
  if (event->button() == Qt::LeftButton)
    m_dragStartPosition = event->globalPos() - frameGeometry().topLeft();
}

void RPWidget::mouseMoveEvent(QMouseEvent *event)
{
  if(!m_dragEnabled) return;
  if (event->buttons() & Qt::LeftButton) {
    move(event->globalPos() - m_dragStartPosition);
  }
}
