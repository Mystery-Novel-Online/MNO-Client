#include "workshop_entry.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QFont>
#include <QLabel>
#include <QMouseEvent>
#include "engine/workshop/workshop_cache.h"
#include <engine/network/api_manager.h>
#include "engine/fs/fs_reading.h"

WorkshopEntry::WorkshopEntry(int id, const QString &iconPath, const QString &title,
                             const QString &subtitle, const QString &genderSymbol,
                             QWidget *parent)
    : QWidget(parent), m_id(id), m_title(title)
{
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  m_rootLayout = new QVBoxLayout(this);
  m_rootLayout->setContentsMargins(0, 0, 0, 0);
  m_rootLayout->setSpacing(0);

  QWidget *headerWidget = new QWidget(this);
  mainLayout = new QHBoxLayout(headerWidget);
  mainLayout->setContentsMargins(1, 1, 1, 1);

  m_IconLabel = new QLabel(headerWidget);
  m_IconLabel->setFixedSize(50, 50);
  mainLayout->addWidget(m_IconLabel);

  m_IconUrl = ApiManager::baseUri() + "api/workshop/" + QString::number(id) + "/icon";

  connect(&WorkshopEntry::iconCache(), &WorkshopCache::fileCached, this, &WorkshopEntry::fileDownloaded);

  WorkshopEntry::iconCache().downloadFile(QUrl(m_IconUrl));

  QVBoxLayout *textLayout = new QVBoxLayout();
  QLabel *titleLabel = new QLabel(title, headerWidget);
  set_stylesheet(titleLabel, "[WORKSHOP NAME]", COURTROOM_STYLESHEETS_CSS,
                 AOApplication::getInstance());

  QLabel *subtitleLabel = new QLabel(subtitle, headerWidget);
  set_stylesheet(subtitleLabel, "[WORKSHOP SUBMITTER]", COURTROOM_STYLESHEETS_CSS,
                 AOApplication::getInstance());

  textLayout->addWidget(titleLabel);
  textLayout->addWidget(subtitleLabel);
  mainLayout->addLayout(textLayout);

  mainLayout->addStretch();

  m_rootLayout->addWidget(headerWidget);

  m_childrenLayout = new QVBoxLayout();
  m_childrenLayout->setContentsMargins(35, 0, 0, 0);
  m_rootLayout->addLayout(m_childrenLayout);

  setLayout(m_rootLayout);
}

WorkshopEntry* WorkshopEntry::createChild(int id, const QString &iconPath, const QString &title, const QString &subtitle, const QString &genderSymbol, QWidget *parent)
{
  auto* childEntry = new WorkshopEntry(id, iconPath, title, subtitle, genderSymbol, parent);
  m_childrenLayout->addWidget(childEntry);
  childEntry->hide();
  return childEntry;
}

void WorkshopEntry::fileDownloaded(const QString &filePath, const QString &hash)
{
  if (filePath.isEmpty())
    return;

  QString expectedHash = WorkshopEntry::iconCache().getHashForUrl(m_IconUrl);
  if (hash == expectedHash)
  {
    QPixmap pix;
    if (pix.load(filePath))
    {
      const QString alphaMaskPath = AOApplication::getInstance()->find_theme_asset_path("workshop_alpha.png");
      if (FS::Checks::FileExists(alphaMaskPath))
      {
        QImage base = pix.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
        QImage mask(alphaMaskPath);

        if (!mask.isNull())
        {
          mask = mask.scaled(base.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation).convertToFormat(QImage::Format_ARGB32_Premultiplied);

          QPainter p(&base);
          p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
          p.drawImage(0, 0, mask);
          p.end();

          pix = QPixmap::fromImage(base);
        }
      }

      m_IconLabel->setPixmap(pix.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

  }
}

void WorkshopEntry::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton) {
    emit rightClicked(m_id);
  }
  if (event->button() == Qt::LeftButton) {
    emit clicked(m_id);

    bool anyVisible = false;
    for (int i = 0; i < m_childrenLayout->count(); ++i) {
      QWidget *w = m_childrenLayout->itemAt(i)->widget();
      if (w && w->isVisible()) {
        anyVisible = true;
        break;
      }
    }

    for (int i = 0; i < m_childrenLayout->count(); ++i) {
      QWidget *w = m_childrenLayout->itemAt(i)->widget();
      if (w) w->setVisible(!anyVisible);
    }
  }
}
