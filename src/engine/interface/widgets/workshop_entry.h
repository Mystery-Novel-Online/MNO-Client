#ifndef WORKSHOP_ENTRY_H
#define WORKSHOP_ENTRY_H

#include <QObject>
#include <QWidget>

class WorkshopEntry : public QWidget
{
  Q_OBJECT
public:
  explicit WorkshopEntry(int id, const QString &iconPath, const QString &title, const QString &subtitle, const QString &genderSymbol, QWidget *parent = nullptr);
  int id() const { return m_id; }

signals:
  void clicked(int id);

protected:
  void mousePressEvent(QMouseEvent *event) override;

private:
  int m_id;
  QString m_title;
};

#endif // WORKSHOP_ENTRY_H
