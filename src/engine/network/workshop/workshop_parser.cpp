#include "workshop_parser.h"

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
