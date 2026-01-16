#ifndef WORKSHOP_ENTRY_H
#define WORKSHOP_ENTRY_H

#include <QObject>
#include <QWidget>
#include "engine/workshop/workshop_cache.h"

class WorkshopEntry : public QWidget
{
  Q_OBJECT

public:
  explicit WorkshopEntry(
      int id,
      const QString &iconPath,
      const QString &title,
      const QString &subtitle,
      const QString &genderSymbol,
      QWidget *parent = nullptr
  );

  WorkshopEntry* createChild(
      int id,
      const QString &iconPath,
      const QString &title,
      const QString &subtitle,
      const QString &genderSymbol,
      QWidget *parent = nullptr
  );

  int id() const { return m_id; }

private slots:
  void fileDownloaded(const QString &filePath, const QString &hash);

signals:
  void clicked(int id);
  void rightClicked(int id);

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  static WorkshopCache& iconCache() { static WorkshopCache instance("cache_data", "cache_data/icon_cache_db.json"); return instance; }
  void setupUi(const QString& title, const QString& subtitle);
  void setupUiGrid(const QString& title, const QString& subtitle);

  void setupIconDownload();
  void applyAlphaMask(QPixmap& pixmap) const;

  bool anyChildrenVisible() const;
  void toggleChildrenVisibility(bool visible);

private:
  int m_id;
  bool m_isGridView = false;
  QString m_title;
  QString m_IconUrl = "";

  QLabel *m_iconLabel = nullptr;

  QVBoxLayout *m_rootLayout = nullptr;
  QHBoxLayout *mainLayout = nullptr;
  QVBoxLayout *m_childrenLayout = nullptr;
};

#endif // WORKSHOP_ENTRY_H
