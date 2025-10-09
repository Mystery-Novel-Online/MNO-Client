#ifndef SERVER_SELECT_ENTRY_H
#define SERVER_SELECT_ENTRY_H

#include <QObject>
#include <QWidget>

class ServerSelectEntry : public QWidget
{
  Q_OBJECT
public:
  ServerSelectEntry(const QString& title, QWidget *parent = nullptr);
  void setId(int id) { m_id = id; }
  int id() const { return m_id; }

signals:
  void clicked(int id);

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  int m_id;
  QString m_title;

  QVBoxLayout *m_rootLayout;
  QHBoxLayout *m_mainLayout;
  QVBoxLayout *m_childrenLayout;
};

#endif // SERVER_SELECT_ENTRY_H
