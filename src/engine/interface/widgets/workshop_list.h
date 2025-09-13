#ifndef WORKSHOP_LIST_H
#define WORKSHOP_LIST_H

#include <QObject>
#include <QWidget>
#include "workshop_entry.h"

struct WorkshopContentEntry
{
  QString name;
  QString submitter;
  QString artist;
  QString description;
  QString downloadLink;
  QString folder;
};

class WorkshopListWidget : public QWidget
{
  Q_OBJECT
public:
  WorkshopListWidget(QWidget *parent = nullptr);
  void addEntry(int id,
                const QString &icon, const QString &title,
                const QString &subtitle, const QString &gender);

  void updateFromApi(const QUrl &url = QUrl("http://localhost:3000/api/workshop"));
  const WorkshopContentEntry getEntry(int id);

signals:
  void entryClicked(int id);

private slots:
  void handleApiReply(QNetworkReply *reply);

private:
  QWidget *m_container;
  QVBoxLayout *m_layout;
  QNetworkAccessManager *m_netManager;

  void clearEntries();

  QMap<int, WorkshopContentEntry> m_EntryData = {};
};

#endif // WORKSHOP_LIST_H
