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


private:
  QGridLayout* m_layout;
  int m_maxCols = 6;

signals:
};


#endif // WORKSHOP_TAGS_H
