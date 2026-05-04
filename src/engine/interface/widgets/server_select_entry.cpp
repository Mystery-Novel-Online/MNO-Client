#include "server_select_entry.h"
#include <QGraphicsDropShadowEffect>

ServerSelectEntry::ServerSelectEntry(const QString& title, QWidget *parent) : QWidget{parent}, m_title(title)
{
  setAttribute(Qt::WA_StyledBackground, true);
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  set_stylesheet(this, "[SERVER ENTRY]", COURTROOM_STYLESHEETS_CSS, AOApplication::getInstance());

  setupLayout();
  createShadow();

}

void ServerSelectEntry::setupLayout()
{
  m_rootLayout = new QVBoxLayout(this);
  m_rootLayout->setContentsMargins(2, 2, 2, 2);
  m_rootLayout->setSpacing(0);

  m_rootLayout->addWidget(createMainRow());

  m_childrenLayout = new QVBoxLayout();
  m_childrenLayout->setContentsMargins(35, 0, 0, 0);
  m_rootLayout->addLayout(m_childrenLayout);
}

QWidget *ServerSelectEntry::createMainRow()
{
  QWidget *mainRow = new QWidget(this);
  mainRow->setStyleSheet("border: none;");

  m_mainLayout = new QHBoxLayout(mainRow);
  m_mainLayout->setContentsMargins(16, 1, 8, 1);

  m_mainLayout->addLayout(createText());
  m_mainLayout->addStretch();
  m_mainLayout->addWidget(createIcon());


  return mainRow;
}

QWidget *ServerSelectEntry::createIcon()
{
  m_iconLabel = new FavoritesLabel(this);
  m_iconLabel->setFixedSize(50, 50);
  m_iconLabel->setStyleSheet("border: none;");

  connect(m_iconLabel, &FavoritesLabel::clicked, this, [this]() {
            emit favoriteToggled(m_id);
          });

  return m_iconLabel;
}

QVBoxLayout *ServerSelectEntry::createText()
{
  QVBoxLayout* layout = new QVBoxLayout();

  m_titleLabel = new QLabel(m_title, this);
  set_stylesheet(m_titleLabel, "[SERVER NAME]", COURTROOM_STYLESHEETS_CSS, AOApplication::getInstance());

  layout->addWidget(m_titleLabel);
  return layout;
}

void ServerSelectEntry::createShadow()
{
  QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(10);
  shadow->setOffset(0, 2);
  shadow->setColor(QColor(0, 0, 0, 160));

  this->setGraphicsEffect(shadow);
}

void ServerSelectEntry::setIcon(QString path)
{
  QPixmap pix;
  if (pix.load(path))
    m_iconLabel->setPixmap(pix.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void ServerSelectEntry::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
    QWidget* child = childAt(event->pos());

    if (child == m_iconLabel)
      return;

    emit clicked(m_id);
  }
}
