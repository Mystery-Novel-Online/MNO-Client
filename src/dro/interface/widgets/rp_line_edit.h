#ifndef RP_LINE_EDIT_H
#define RP_LINE_EDIT_H

#include <QLineEdit>

class AOLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  AOLineEdit(QWidget *parent);

protected:
  void mouseDoubleClickEvent(QMouseEvent *e);

signals:
  void double_clicked();

private slots:
  void on_enter_pressed();
};

#endif // RP_LINE_EDIT_H
