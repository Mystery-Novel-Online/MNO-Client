#include "workshop_entry.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QFont>
#include <QLabel>
#include <QMouseEvent>
#include "engine/workshop/workshop_cache.h"

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

  QLabel *iconLabel = new QLabel(headerWidget);
  iconLabel->setFixedSize(50, 50);
  mainLayout->addWidget(iconLabel);

  const QString workshopUrl =
      QString::fromStdString(config::ConfigUserSettings::stringValue(
          "workshop_url", "http://localhost:3623/")) +
      "api/workshop/" + QString::number(id) + "/icon";

  connect(&WorkshopEntry::iconCache(), &WorkshopCache::fileCached, this,
          [iconLabel, workshopUrl](const QString &filePath, const QString &hash) {
            if (filePath.isEmpty())
              return;

            QString expectedHash = WorkshopEntry::iconCache().getHashForUrl(workshopUrl);
            if (hash == expectedHash) {
              QPixmap pix;
              if (pix.load(filePath)) {
                iconLabel->setPixmap(
                    pix.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
              }
            }
          });

  WorkshopEntry::iconCache().downloadFile(QUrl(workshopUrl));

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
