#include "workshop_entry.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QFont>
#include <QLabel>
#include <QMouseEvent>
#include "engine/workshop/workshop_cache.h"
#include <engine/network/api_manager.h>
#include "engine/fs/fs_reading.h"

constexpr int ICON_CONTAINER_SIZE = 50;
constexpr int ICON_RENDER_SIZE    = 48;
constexpr int CHILD_INDENT        = 35;

WorkshopEntry::WorkshopEntry(int id, const QString&, const QString &title, const QString &subtitle, const QString&, QWidget *parent) : QWidget(parent), m_id(id), m_title(title)
{
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  setupUi(title, subtitle);
  setupIconDownload();
}

WorkshopEntry* WorkshopEntry::createChild(int id, const QString &iconPath, const QString &title, const QString &subtitle, const QString &genderSymbol, QWidget *parent)
{
  auto* child = new WorkshopEntry(id, iconPath, title, subtitle, genderSymbol, parent);
  m_childrenLayout->addWidget(child);
  child->hide();
  return child;
}

void WorkshopEntry::fileDownloaded(const QString &filePath, const QString &hash)
{
  if (filePath.isEmpty())
    return;

  if (hash != WorkshopEntry::iconCache().getHashForUrl(m_IconUrl))
    return;

  QPixmap pixmap;
  if (!pixmap.load(filePath))
    return;

  applyAlphaMask(pixmap);

  m_iconLabel->setPixmap(
      pixmap.scaled(
          ICON_RENDER_SIZE,
          ICON_RENDER_SIZE,
          Qt::KeepAspectRatio,
          Qt::SmoothTransformation
          )
      );
}

void WorkshopEntry::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::RightButton)
  {
    emit rightClicked(m_id);
    return;
  }

  if (event->button() == Qt::LeftButton)
  {
    emit clicked(m_id);
    toggleChildrenVisibility(!anyChildrenVisible());
  }
}

void WorkshopEntry::setupUi(const QString &title, const QString &subtitle)
{
  m_rootLayout = new QVBoxLayout(this);
  m_rootLayout->setContentsMargins(0, 0, 0, 0);
  m_rootLayout->setSpacing(0);

  auto* headerWidget = new QWidget(this);
  auto* headerLayout = new QHBoxLayout(headerWidget);
  headerLayout->setContentsMargins(1, 1, 1, 1);

  m_iconLabel = new QLabel(headerWidget);
  m_iconLabel->setFixedSize(ICON_CONTAINER_SIZE, ICON_CONTAINER_SIZE);
  headerLayout->addWidget(m_iconLabel);

  auto* textLayout = new QVBoxLayout();

  auto* titleLabel = new QLabel(title, headerWidget);
  set_stylesheet(titleLabel, "[WORKSHOP NAME]", COURTROOM_STYLESHEETS_CSS, AOApplication::getInstance());

  auto* subtitleLabel = new QLabel(subtitle, headerWidget);
  set_stylesheet(subtitleLabel, "[WORKSHOP SUBMITTER]", COURTROOM_STYLESHEETS_CSS, AOApplication::getInstance());

  textLayout->addWidget(titleLabel);
  textLayout->addWidget(subtitleLabel);

  headerLayout->addLayout(textLayout);
  headerLayout->addStretch();

  m_rootLayout->addWidget(headerWidget);

  m_childrenLayout = new QVBoxLayout();
  m_childrenLayout->setContentsMargins(CHILD_INDENT, 0, 0, 0);
  m_rootLayout->addLayout(m_childrenLayout);
}

void WorkshopEntry::setupIconDownload()
{
  m_IconUrl = ApiManager::baseUri() + "api/workshop/" + QString::number(m_id) + "/icon";
  connect(&WorkshopEntry::iconCache(), &WorkshopCache::fileCached, this, &WorkshopEntry::fileDownloaded);
  WorkshopEntry::iconCache().downloadFile(QUrl(m_IconUrl));
}

void WorkshopEntry::applyAlphaMask(QPixmap &pixmap) const
{
  const QString maskPath = AOApplication::getInstance()->find_theme_asset_path("workshop_alpha.png");

  if (!FS::Checks::FileExists(maskPath))
    return;

  QImage base = pixmap.toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);

  QImage mask(maskPath);

  if (mask.isNull())
    return;

  mask = mask
             .scaled(base.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)
             .convertToFormat(QImage::Format_ARGB32_Premultiplied);

  QPainter painter(&base);
  painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
  painter.drawImage(0, 0, mask);

  pixmap = QPixmap::fromImage(base);
}

bool WorkshopEntry::anyChildrenVisible() const
{
  for (int i = 0; i < m_childrenLayout->count(); ++i)
  {
    if (auto* w = m_childrenLayout->itemAt(i)->widget();
        w && w->isVisible())
    {
      return true;
    }
  }
  return false;
}

void WorkshopEntry::toggleChildrenVisibility(bool visible)
{
  for (int i = 0; i < m_childrenLayout->count(); ++i)
  {
    if (auto* w = m_childrenLayout->itemAt(i)->widget())
      w->setVisible(visible);
  }
}
