#ifndef WORKSHOP_PARSER_H
#define WORKSHOP_PARSER_H
#include "workshop_models.h"

class WorkshopParser
{
public:
  static WorkshopCollection parseCollection(const QByteArray& json);
  static QList<WorkshopCollection> parseCollections(const QByteArray &json);

  static WorkshopRepository parseRepository(const QJsonObject& object);
  static WorkshopFile parseFile(const QJsonObject& object);
  static WorkshopContentEntry parseEntry(const QJsonObject& object);

  static WorkshopContentEntry requestDetails(const QString& guid, WorkshopContentEntry& target);


};

#endif // WORKSHOP_PARSER_H
