#include "server_select_entry.h"
#include "serverselectlist.h"

ServerSelectList::ServerSelectList(QWidget *parent) : QWidget{parent}
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setStyleSheet("background-color: transparent; border: none; color: yellow;");

  m_container = new QWidget(scrollArea);
  m_layout = new QVBoxLayout(m_container);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setMargin(0);

  scrollArea->setWidget(m_container);

  QVBoxLayout *rootLayout = new QVBoxLayout(this);
  rootLayout->addWidget(scrollArea);
  setLayout(rootLayout);
}

void ServerSelectList::addEntry(QUrl icon, QString title)
{

}

void ServerSelectList::addEntry(QString title)
{
  ServerSelectEntry *entry = new ServerSelectEntry(title);
  connect(entry, &ServerSelectEntry::clicked, this, &ServerSelectList::entryClicked);
  m_layout->addWidget(entry);
}
