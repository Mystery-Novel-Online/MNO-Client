#ifndef NOTE_PICKER_H
#define NOTE_PICKER_H

class AOApplication;
class RPButton;

#include <QLabel>

class QHBoxLayout;
class QLineEdit;

class AONotePicker : public QLabel
{
  Q_OBJECT

public:
  AONotePicker(QWidget *p_parent, AOApplication *p_ao_app);

  QString m_file;

  QLineEdit *ui_line = nullptr;
  RPButton *ui_button = nullptr;
  RPButton *ui_delete_button = nullptr;
  RPButton *ui_hover = nullptr;
  QHBoxLayout *ui_layout = nullptr;

  void set_active(bool p_active);
  bool is_active();

private:
  AOApplication *ao_app = nullptr;

  bool m_active = false;
};

#endif // NOTE_PICKER_H
