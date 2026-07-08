#include "workshop_parser.h"

#include <engine/network/api_manager.h>

WorkshopCollection WorkshopParser::parseCollection(const QByteArray &json)
{
  WorkshopCollection collection;

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(json, &error);

  if (error.error != QJsonParseError::NoError || !doc.isObject())
  {
    qWarning() << "Failed to parse workshop collection:" << error.errorString();
    return collection;
  }

  QJsonObject root = doc.object();

  collection.name = root["collection_name"].toString();
  collection.sizeBytes = root["file_size"].toInt();

  if (root.contains("contents"))
  {
    collection.repositories.append(parseRepository(root));
    return collection;
  }

  if (root.contains("repos") && root["repos"].isArray())
  {
    QJsonArray repos = root["repos"].toArray();

    for (const QJsonValue& value : repos)
    {
      if (!value.isObject())
        continue;

      collection.repositories.append(parseRepository(value.toObject()));
    }
  }

  return collection;
}

QList<WorkshopCollection> WorkshopParser::parseCollections(const QByteArray &json)
{
  QList<WorkshopCollection> collections;

  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(json, &error);

  if (error.error != QJsonParseError::NoError || !doc.isArray())
  {
    qWarning() << "Failed to parse workshop collections:" << error.errorString();
    return collections;
  }

  QJsonArray root = doc.array();

  for (const QJsonValue &value : root)
  {
    if (!value.isObject())
      continue;

    QJsonObject object = value.toObject();

    WorkshopCollection collection;

    collection.name = object["collection_name"].toString();
    collection.sizeBytes = object["file_size"].toInt();

    if (object.contains("contents"))
    {
      collection.repositories.append(parseRepository(object));
    }
    else if (object.contains("repos") && object["repos"].isArray())
    {
      QJsonArray repos = object["repos"].toArray();

      for (const QJsonValue &repo : repos)
      {
        if (!repo.isObject())
          continue;

        collection.repositories.append(parseRepository(repo.toObject()));
      }
    }

    collections.append(collection);
  }

  return collections;
}

WorkshopRepository WorkshopParser::parseRepository(const QJsonObject &object)
{
  WorkshopRepository repo;

  repo.guid = object["guid"].toString();
  repo.folderName = object["folder"].toString();
  repo.downloadType = object["url_download"].toString();

  repo.contentId = object["id"].toInt();
  repo.lastUpdated = object["last_updated"].toInt();

  if (object.contains("contents") && object["contents"].isArray())
  {
    QJsonArray files = object["contents"].toArray();

    for (const QJsonValue& value : files)
    {
      if (!value.isObject())
        continue;

      repo.files.append(parseFile(value.toObject()));
    }
  }

  return repo;
}

WorkshopFile WorkshopParser::parseFile(const QJsonObject &object)
{
  WorkshopFile file;

  file.relativePath = object["file_path"].toString();
  file.hash = object["hash"].toString();

  return file;
}

WorkshopContentEntry WorkshopParser::parseEntry(const QJsonObject &obj)
{
  WorkshopContentEntry entry;

  entry.id = obj.value("id").toInt();

  entry.content_type = obj.value("url_download").toString();
  entry.guid = obj.value("guid").toString();

  if(entry.content_type.isEmpty() || entry.content_type == "repo" || entry.content_type == "background")
    entry.downloadLink = ApiManager::baseUri() + "api/workshop/" + entry.guid + "/content";

  if(entry.content_type == "collection")
    entry.downloadLink = ApiManager::baseUri() + "api/workshop/" + QString::number(entry.id) + "/collection";

  entry.submitter = obj.value("submitter").toString();
  entry.submitterText = entry.submitter;

  for(auto collab : obj.value("collaborators").toArray())
  {
    QJsonObject collabObj = collab.toObject();
    int user_id = collabObj.value("user_id").toInt(0);
    int permissions = collabObj.value("permissions").toInt(0);
    QString username = collabObj.value("username").toString();

    entry.collaborators.append({user_id, username, permissions});

    entry.submitterText += ", ";
    entry.submitterText += username;
  }


  QMap<int, std::string> categories = ApiManager::instance().categoryMap();
  for(auto tag : obj.value("tags").toArray())
  {
    QJsonObject tagObj = tag.toObject();
    QString value = tagObj.value("name").toString();
    int key = tagObj.value("category").toInt(0);
    if(categories.contains(key))
    {
      entry.tagMap.append({QString::fromStdString(categories[key]), value});
    }
  }

  entry.name = obj.value("name").toString();
  entry.description = obj.value("description").toString();
  entry.folder = obj.value("folder").toString();

  auto childrenArray = obj.value("children").toArray();

  for(const auto & child : childrenArray)
  {
    WorkshopContentEntry newEntry = parseEntry(child.toObject());
    entry.children.append(newEntry);
  }


  return entry;
}
