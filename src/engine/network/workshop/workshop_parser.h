#ifndef WORKSHOP_PARSER_H
#define WORKSHOP_PARSER_H
#include "workshop_models.h"

class WorkshopParser
{
public:
  static WorkshopCollection parseCollection(const QByteArray& json);
  static WorkshopRepository parseRepository(const QJsonObject& object);
  static WorkshopFile parseFile(const QJsonObject& object);
  static WorkshopContentEntry parseEntry(const QJsonObject& object);
};

#endif // WORKSHOP_PARSER_H
