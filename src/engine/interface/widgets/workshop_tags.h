#ifndef WORKSHOP_TAGS_H
#define WORKSHOP_TAGS_H

#include <QObject>

class WorkshopTags : public QWidget
{
  Q_OBJECT
public:
  explicit WorkshopTags(QWidget *parent = nullptr);
  ~WorkshopTags();

  void addTag(const QString& text);
  void setMaxColumns(int cols) { m_maxCols = cols; };
  void clearAllTags();


private:
  QScrollArea* m_scrollArea = nullptr;
  QWidget* m_container = nullptr;
  QHBoxLayout* m_layout = nullptr;

  int m_maxCols = 6;

signals:
};


#endif // WORKSHOP_TAGS_H
