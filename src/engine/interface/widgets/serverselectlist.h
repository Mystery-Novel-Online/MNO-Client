#ifndef SERVERSELECTLIST_H
#define SERVERSELECTLIST_H

#include <QObject>
#include <QWidget>

class ServerSelectList : public QWidget
{
  Q_OBJECT
public:
  ServerSelectList(QWidget *parent = nullptr);
  void clearEntries();
  void addEntry(QUrl icon, QString title);
  void addEntry(QString title);

  void setHidden(int id, bool state);

  void setIcon(int id, QString path);

signals:
  void entryClicked(int index);

private:
  QWidget *m_container;
  QVBoxLayout *m_layout;
};

#endif // SERVERSELECTLIST_H
