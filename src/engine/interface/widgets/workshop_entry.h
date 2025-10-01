#ifndef WORKSHOP_ENTRY_H
#define WORKSHOP_ENTRY_H

#include <QObject>
#include <QWidget>
#include "engine/workshop/workshop_cache.h"

class WorkshopEntry : public QWidget
{
  Q_OBJECT
public:
  explicit WorkshopEntry(int id, const QString &iconPath, const QString &title, const QString &subtitle, const QString &genderSymbol, QWidget *parent = nullptr);
  WorkshopEntry* createChild(int id, const QString &iconPath, const QString &title, const QString &subtitle, const QString &genderSymbol, QWidget *parent = nullptr);
  int id() const { return m_id; }

  static WorkshopCache& iconCache() {
    static WorkshopCache instance("cache_data", "cache_data/icon_cache_db.json");
    return instance;
  }

signals:
  void clicked(int id);

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  int m_id;
  QString m_title;


  QVBoxLayout *m_rootLayout;
  QHBoxLayout *mainLayout;
  QVBoxLayout *m_childrenLayout;
};

#endif // WORKSHOP_ENTRY_H
