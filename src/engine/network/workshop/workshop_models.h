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

  QString packageDirectory() {
    QString inputName = name.replace(QRegularExpression(R"([<>:"/\\|?*])"), "");

    inputName = inputName.replace(QRegularExpression(R"([<>:"/\\|?*\x00-\x1F])"), " ");

    while (inputName.endsWith(' ') || inputName.endsWith('.'))
      inputName.chop(1);

    if (inputName.isEmpty())
      inputName = "Workshop Downloads";

    return "packages/" + name + "/";
  }
};

struct WorkshopCollaborator
{
  int user_id;
  QString username;
  int permissions;
};

struct WorkshopContentEntry
{
  int id;
  QString name;
  QString submitter;
  QString submitterText;
  QString description;
  QString downloadLink;
  QString content_type;
  QString folder;
  QVector<QPair<QString, QString>> tagMap;
  QVector<WorkshopCollaborator> collaborators;
  QString guid;
  QVector<WorkshopContentEntry> children;
};
