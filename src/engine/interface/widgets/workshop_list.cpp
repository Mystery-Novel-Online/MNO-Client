#include "workshop_list.h"

WorkshopListWidget::WorkshopListWidget(QWidget *parent) : QWidget(parent)
{
  QScrollArea *scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);

  m_container = new QWidget(scrollArea);
  m_layout = new QVBoxLayout(m_container);
  m_layout->setAlignment(Qt::AlignTop);
  m_layout->setMargin(0);

  scrollArea->setWidget(m_container);

  //Setup the layout that the entrys will be added to.
  QVBoxLayout *rootLayout = new QVBoxLayout(this);
  rootLayout->addWidget(scrollArea);
  setLayout(rootLayout);

  //Setup API Manager
  m_netManager = new QNetworkAccessManager(this);
  connect(m_netManager, &QNetworkAccessManager::finished, this, &WorkshopListWidget::handleApiReply);
  scrollArea->setStyleSheet("background-color: transparent; border: none; color: yellow;");
}

void WorkshopListWidget::addEntry(int id, const QString &icon, const QString &title, const QString &subtitle, const QString &gender, const QJsonArray &children)
{
  WorkshopEntry *entry = new WorkshopEntry(id, icon, title, subtitle, gender, m_container);
  connect(entry, &WorkshopEntry::clicked, this, &WorkshopListWidget::entryClicked);
  connect(entry, &WorkshopEntry::rightClicked, this, &WorkshopListWidget::entryRightClicked);
  m_layout->addWidget(entry);
  for(const auto & child : children)
  {
    int id = child.toObject().value("id").toInt();
    QString url = child.toObject().value("url_download").toString();
    if(url.isEmpty() || url == "repo")
    {
      url = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + "api/workshop/" + QString::number(id) + "/repo";
    };

    WorkshopContentEntry newEntry = {child.toObject().value("name").toString(), child.toObject().value("submitter").toString(), child.toObject().value("artist").toString(), child.toObject().value("description").toString(), url, child.toObject().value("folder").toString()};
    auto childWidget = entry->createChild(id, "", newEntry.name, newEntry.submitter, "", nullptr);
    connect(childWidget, &WorkshopEntry::clicked, this, &WorkshopListWidget::entryClicked);
    m_EntryData[id] = newEntry;
  }
}

void WorkshopListWidget::updateFromApi(const QString &category)
{
  clearEntries();
  QString workshopPath = "api/workshop";

  if(category == "portfolio")
  {
    workshopPath += "/my_uploads?key=" + QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_key", "PUT_KEY_HERE"));
  }
  else
  {
    workshopPath += "/" + category;
  }


  const QString workshopUrl = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + workshopPath;
  const QUrl url = QUrl(workshopUrl);
  QNetworkRequest request(url);
  m_netManager->get(request);
}

const WorkshopContentEntry WorkshopListWidget::getEntry(int id)
{
  if(m_EntryData.contains(id)) return m_EntryData[id];
  return {};
}

void WorkshopListWidget::updateSearch(QString search)
{
  QString searchProcessed = search.toLower();

  for (int i = 0; i < m_layout->count(); ++i)
  {
    QLayoutItem *child = m_layout->itemAt(i);
    if (!child) continue;

    if (QWidget *w = child->widget())
    {
      if (WorkshopEntry *entry = dynamic_cast<WorkshopEntry *>(w))
      {
        WorkshopContentEntry entryData = getEntry(entry->id());
        QString nameProcessed = entryData.name.toLower();

        if (nameProcessed.contains(searchProcessed))
          entry->show();
        else
          entry->hide();
      }
    }
  }

  m_layout->update();
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
  if (!doc.isArray())
  {
    qWarning() << "Invalid JSON received!";
    return;
  }

  m_EntryData.clear();

  QJsonArray arr = doc.array();
  for (const QJsonValue &val : arr) {
    if (!val.isObject()) continue;

    QJsonObject obj = val.toObject();
    int id = obj.value("id").toInt();
    QString url = obj.value("url_download").toString();
    if(url.isEmpty() || url == "repo")
    {
      url = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + "api/workshop/" + QString::number(id) + "/repo";
    };
    if(url == "collection")
    {
      url = QString::fromStdString(config::ConfigUserSettings::stringValue("workshop_url", "http://localhost:3623/")) + "api/workshop/" + QString::number(id) + "/collection";
    }

    WorkshopContentEntry newEntry = {obj.value("name").toString(), obj.value("submitter").toString(), obj.value("artist").toString(), obj.value("description").toString(), url, obj.value("folder").toString()};
    QString iconUrl = obj.value("url_icon").toString();

    auto childrenArray = obj.value("children").toArray();
    addEntry(id, iconUrl, newEntry.name, newEntry.submitter, "♀", childrenArray);
    m_EntryData[id] = newEntry;
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
