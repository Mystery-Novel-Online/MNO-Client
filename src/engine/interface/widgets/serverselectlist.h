#ifndef SERVERSELECTLIST_H
#define SERVERSELECTLIST_H

#include <QObject>
#include <QWidget>

class ServerSelectList : public QWidget
{
  Q_OBJECT
public:
  ServerSelectList(QWidget *parent = nullptr);
  void addEntry(QUrl icon, QString title);
  void addEntry(QString title);
  void setHidden(int id, bool state);
  void showEntry(int id);
  void hideEntry(int id);

  void clearEntries();
signals:
  void entryClicked(int index);

private:
  QWidget *m_container;
  QVBoxLayout *m_layout;
};

#endif // SERVERSELECTLIST_H
