#include "workshop_entry.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QFont>
#include "engine/workshop/workshop_cache.h"

WorkshopEntry::WorkshopEntry(int id, const QString &iconPath, const QString &title, const QString &subtitle, const QString &genderSymbol, QWidget *parent) : QWidget(parent), m_id(id), m_title(title)
{
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  QHBoxLayout *mainLayout = new QHBoxLayout(this);
  mainLayout->setContentsMargins(1, 1, 1, 1);

  QLabel *iconLabel = new QLabel(this);
  iconLabel->setFixedSize(50, 50);
  mainLayout->addWidget(iconLabel);

  QNetworkAccessManager *manager = new QNetworkAccessManager(this);

  const QString workshopUrl = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + "api/workshop/" + QString::number(id) + "/icon";

  connect(&WorkshopEntry::iconCache(), &WorkshopCache::fileCached, this,
          [iconLabel, workshopUrl](const QString &filePath, const QString &hash) {
            if (filePath.isEmpty()) return;

            QString expectedHash = WorkshopEntry::iconCache().getHashForUrl(workshopUrl);
            if (hash == expectedHash) {
              QPixmap pix;
              if (pix.load(filePath)) {
                iconLabel->setPixmap(pix.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
              }
            }
          });

  WorkshopEntry::iconCache().downloadFile(QUrl(workshopUrl));


  QVBoxLayout *textLayout = new QVBoxLayout();
  QLabel *titleLabel = new QLabel(title, this);
  set_stylesheet(titleLabel, "[WORKSHOP NAME]", COURTROOM_STYLESHEETS_CSS, AOApplication::getInstance());

  QLabel *subtitleLabel = new QLabel(subtitle, this);
  set_stylesheet(subtitleLabel, "[WORKSHOP SUBMITTER]", COURTROOM_STYLESHEETS_CSS, AOApplication::getInstance());

  textLayout->addWidget(titleLabel);
  textLayout->addWidget(subtitleLabel);
  mainLayout->addLayout(textLayout);

  mainLayout->addStretch();

  setLayout(mainLayout);
}

void WorkshopEntry::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    emit clicked(m_id);
}
