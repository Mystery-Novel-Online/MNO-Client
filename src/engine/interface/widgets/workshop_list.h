#ifndef WORKSHOP_LIST_H
#define WORKSHOP_LIST_H

#include <QObject>
#include <QWidget>
#include "workshop_entry.h"

struct WorkshopCollaborator
{
  int user_id;
  QString username;
  int permissions;
};

struct WorkshopContentEntry
{
  QString name;
  QString submitter;
  QString artist;
  QString description;
  QString downloadLink;
  QString folder;
  QVector<QPair<QString, QString>> tagMap;
  QVector<WorkshopCollaborator> collaborators;
};


class WorkshopListWidget : public QWidget
{
  Q_OBJECT
public:
  WorkshopListWidget(QWidget *parent = nullptr);
  void addEntry(int id,
                const QString &icon, const QString &title,
                const QString &subtitle, const QString &gender, const QJsonArray &children);

  void updateFromApi(const QString& category = "browse");
  const WorkshopContentEntry getEntry(int id);

  void setPageNumber(int page) { m_pageCurrent = page; }
  void nextPage();
  void previousPage();

  int totalPages() { return m_pageTotal; }
  int currentPage() { return m_pageCurrent; }

signals:
  void entryClicked(int id);
  void entryRightClicked(int id);
  void contentParsed(int current, int total);

private slots:
  void handleApiReply(QNetworkReply *reply);

private:
  QWidget *m_container;
  QVBoxLayout *m_layout;
  QNetworkAccessManager *m_netManager;

  void clearEntries();

  QMap<int, WorkshopContentEntry> m_EntryData = {};

  QString m_currentCategory = "browse";

  int m_pageCurrent = 1;
  int m_pageTotal = 0;


};

#endif // WORKSHOP_LIST_H
