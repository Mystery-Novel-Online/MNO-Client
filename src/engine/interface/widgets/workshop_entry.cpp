#include "workshop_entry.h"
#include <QPixmap>
#include <QVBoxLayout>
#include <QFont>

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
  QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(workshopUrl)));

  connect(reply, &QNetworkReply::finished, this, [reply, iconLabel]()
  {
    if(reply->error() == QNetworkReply::NoError) {
      QByteArray imageData = reply->readAll();
      QPixmap pix;
      if(pix.loadFromData(imageData)) iconLabel->setPixmap(pix.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      else qWarning("Failed to load image from data");
    }
    else
      qWarning() << "Failed to download image:" << reply->errorString();

    reply->deleteLater();
  });

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
