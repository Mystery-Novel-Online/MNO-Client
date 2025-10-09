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

void ServerSelectList::showEntry(int id)
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
          entry->show();
          m_layout->update();
          return;
        }
      }
    }
  }
}

void ServerSelectList::hideEntry(int id)
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
          entry->hide();
          m_layout->update();
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
