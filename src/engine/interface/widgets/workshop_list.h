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
                const QString &subtitle, const QString &gender, const QJsonArray &children);

  void updateFromApi(const QString& category = "browse");
  const WorkshopContentEntry getEntry(int id);

signals:
  void entryClicked(int id);
  void entryRightClicked(int id);

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
