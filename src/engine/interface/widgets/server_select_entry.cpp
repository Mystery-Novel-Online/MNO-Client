#include "server_select_entry.h"

ServerSelectEntry::ServerSelectEntry(const QString& title, QWidget *parent) : QWidget{parent}, m_title(title)
{
  setCursor(Qt::PointingHandCursor);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  m_rootLayout = new QVBoxLayout(this);
  m_rootLayout->setContentsMargins(0, 0, 0, 0);
  m_rootLayout->setSpacing(0);

  QWidget *headerWidget = new QWidget(this);
  m_mainLayout = new QHBoxLayout(headerWidget);
  m_mainLayout->setContentsMargins(1, 1, 1, 1);

  QLabel *iconLabel = new QLabel(headerWidget);
  iconLabel->setFixedSize(50, 50);
  m_mainLayout->addWidget(iconLabel);

  QVBoxLayout *textLayout = new QVBoxLayout();
  QLabel *titleLabel = new QLabel(title, headerWidget);
  set_stylesheet(titleLabel, "[WORKSHOP NAME]", COURTROOM_STYLESHEETS_CSS, AOApplication::getInstance());
  textLayout->addWidget(titleLabel);

  m_mainLayout->addLayout(textLayout);

  m_mainLayout->addStretch();

  m_rootLayout->addWidget(headerWidget);

  m_childrenLayout = new QVBoxLayout();
  m_childrenLayout->setContentsMargins(35, 0, 0, 0);
  m_rootLayout->addLayout(m_childrenLayout);

  setLayout(m_rootLayout);
}

void ServerSelectEntry::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton) {
    emit clicked(m_id);
  }
}
