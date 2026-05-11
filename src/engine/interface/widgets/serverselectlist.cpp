#include "server_select_entry.h"
#include "serverselectlist.h"

ServerSelectList::ServerSelectList(QWidget *parent) : QWidget{parent}
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);

  scrollArea->viewport()->setStyleSheet("background: transparent; border: none;");
  set_stylesheet(scrollArea, "[SERVER LIST]", LOBBY_STYLESHEETS_CSS, AOApplication::getInstance());

  m_container = new QWidget(scrollArea);
  m_layout = new QVBoxLayout(m_container);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setMargin(16);

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
  connect(entry, &ServerSelectEntry::favoriteToggled, this, &ServerSelectList::favoriteClicked);
  entry->setId(m_layout->count());
  m_layout->addWidget(entry);
}

void ServerSelectList::setHidden(int id, bool state)
{
  for (int i = 0; i < m_layout->count(); ++i)
  {
    QLayoutItem *child = m_layout->itemAt(i);
    if (!child) continue;

    if (QWidget *w = child->widget())
    {
      if (ServerSelectEntry *entry = dynamic_cast<ServerSelectEntry *>(w))
      {
        if (entry->id() == id)
        {
          entry->setHidden(state);
          m_layout->update();
          return;
        }
      }
    }
  }
}

void ServerSelectList::setIcon(int id, QString path)
{
  for (int i = 0; i < m_layout->count(); ++i)
  {
    QLayoutItem *child = m_layout->itemAt(i);
    if (!child) continue;

    if (QWidget *w = child->widget())
    {
      if (ServerSelectEntry *entry = dynamic_cast<ServerSelectEntry *>(w))
      {
        if (entry->id() == id)
        {
          entry->setIcon(path);
          return;
        }
      }
    }
  }
}

void ServerSelectList::clearEntries()
{
  QLayoutItem *child;
  while ((child = m_layout->takeAt(0)) != nullptr) {
    if (QWidget *w = child->widget())
      w->deleteLater();
    delete child;
  }
}
