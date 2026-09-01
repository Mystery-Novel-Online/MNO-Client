#ifndef WORKSHOPLISTWIDGET_H
#define WORKSHOPLISTWIDGET_H

#include <QObject>
#include <QWidget>
#include "WorkshopEntry.h"
#include "network/workshop/workshop_models.h"


class WorkshopListWidget : public QWidget
{
  Q_OBJECT
public:
  WorkshopListWidget(QWidget *parent = nullptr);
  void addEntry(const WorkshopContentEntry& entryData);

  void updateFromApi(const QString& category = "browse");
  WorkshopContentEntry& getEntry(int id);

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

#endif // WORKSHOPLISTWIDGET_H
