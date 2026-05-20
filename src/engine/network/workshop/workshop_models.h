#pragma once

#include <QString>
#include <QList>

enum class WorkshopContentType
{
  Character,
  Background,
  Collection
};

struct WorkshopFile
{
  QString relativePath;
  QString hash;
};

struct WorkshopRepository
{
  int contentId = 0;
  int lastUpdated = 0;

  QString guid;
  QString folderName;
  QString downloadType;

  QList<WorkshopFile> files;
};

struct WorkshopCollection
{
  QString name;
  QList<WorkshopRepository> repositories;

  QString packageDirectory() { return "packages/" + name + "/"; }
};
