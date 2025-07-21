#pragma once



#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>

class DRServerInfoEditor : public QDialog
{
  Q_OBJECT

public:
  DRServerInfoEditor(QWidget *parent = nullptr);
  ~DRServerInfoEditor();

  DRServerInfo get_server_info();

public slots:
  void set_server_info(DRServerInfo p_server_info);
  void clear_server_info();

private:
  QLineEdit *ui_name;
  QPlainTextEdit *ui_description;
  QLineEdit *ui_address;
  QSpinBox *ui_port;
  QDialogButtonBox *ui_button_box;
};
