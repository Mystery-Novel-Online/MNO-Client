#include "workshop_list.h"
#include "engine/network/workshop/workshop_parser.h"

#include <engine/network/api_manager.h>

WorkshopListWidget::WorkshopListWidget(QWidget *parent) : QWidget(parent)
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);

  m_container = new QWidget(scrollArea);
  m_layout = new QVBoxLayout(m_container);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setMargin(0);
  m_layout->setSpacing(4);

  scrollArea->setWidget(m_container);

  //Setup the layout that the entrys will be added to.
  QVBoxLayout *rootLayout = new QVBoxLayout(this);
  rootLayout->addWidget(scrollArea);
  setLayout(rootLayout);

  //Setup API Manager
  m_netManager = new QNetworkAccessManager(this);
  connect(m_netManager, &QNetworkAccessManager::finished, this, &WorkshopListWidget::handleApiReply);

  scrollArea->viewport()->setStyleSheet("background: transparent; border: none;");
  set_stylesheet(scrollArea, "[WORKSHOP LIST]", LOBBY_STYLESHEETS_CSS, AOApplication::getInstance());
}

void WorkshopListWidget::addEntry(const WorkshopContentEntry &entryData)
{
  WorkshopEntry *entry = new WorkshopEntry(entryData, m_container);
  connect(entry, &WorkshopEntry::clicked, this, &WorkshopListWidget::entryClicked);
  connect(entry, &WorkshopEntry::rightClicked, this, &WorkshopListWidget::entryRightClicked);
  m_layout->addWidget(entry);

  QMap<int, std::string> categories = ApiManager::instance().categoryMap();

  for(const auto & childEntry : entryData.children)
  {
    auto childWidget = entry->createChild(childEntry, nullptr);
    connect(childWidget, &WorkshopEntry::clicked, this, &WorkshopListWidget::entryClicked);
    m_EntryData[childEntry.id] = childEntry;
  }
}

void WorkshopListWidget::updateFromApi(const QString &category)
{
  clearEntries();

  m_currentCategory = category;

  QString path = "api/workshop";
  const QString apiKey = ApiManager::authorizationKey();

  if(category == "portfolio")
    path += "/my_uploads?key=" + apiKey;
  else if(category == "pending")
    path += "/verification_queue?key=" + apiKey;
  else
  {
    if(category.contains("search"))
      path += "/" + category + "&page=" + QString::number(m_pageCurrent) + "&key=" + apiKey;
    else
      path += "/" + category + "?page=" + QString::number(m_pageCurrent) + "&key=" + apiKey;
  }

  const QUrl url = QUrl(ApiManager::baseUri() + path);
  m_netManager->get(QNetworkRequest(url));
}

const WorkshopContentEntry WorkshopListWidget::getEntry(int id)
{
  if(m_EntryData.contains(id)) return m_EntryData[id];
  return {};
}

void WorkshopListWidget::nextPage()
{
  if(m_pageCurrent >= m_pageTotal)
    return;

  m_pageCurrent += 1;
  updateFromApi(m_currentCategory);
}

void WorkshopListWidget::previousPage()
{
  if(m_pageCurrent <= 1)
    return;

  m_pageCurrent -= 1;
  updateFromApi(m_currentCategory);
}


void WorkshopListWidget::handleApiReply(QNetworkReply *reply)
{
  if (reply->error() != QNetworkReply::NoError)
  {
    qWarning() << "API request failed:" << reply->errorString();
    reply->deleteLater();
    return;
  }

  QByteArray responseData = reply->readAll();
  reply->deleteLater();

  QJsonDocument doc = QJsonDocument::fromJson(responseData);
  if (!doc.object().value("contents").isArray())
  {
    qWarning() << "Invalid JSON received!";
    return;
  }

  m_EntryData.clear();
  clearEntries();


  QMap<int, std::string> categories = ApiManager::instance().categoryMap();

  m_pageCurrent = doc.object().value("page_current").toInt();
  m_pageTotal = doc.object().value("page_total").toInt();
  QJsonArray arr = doc.object().value("contents").toArray();
  for (const QJsonValue &val : arr)
  {
    if (!val.isObject()) continue;

    QJsonObject obj = val.toObject();
    WorkshopContentEntry newEntry = WorkshopParser::parseEntry(obj);

    addEntry(newEntry);
    m_EntryData[newEntry.id] = newEntry;

    emit contentParsed(m_pageCurrent, m_pageTotal);
  }
}

void WorkshopListWidget::clearEntries()
{
  QLayoutItem *child;
  while ((child = m_layout->takeAt(0)) != nullptr) {
    if (QWidget *w = child->widget())
      w->deleteLater();
    delete child;
  }
}
