#include "drserverinfoeditor.h"
#include "pch.h"
#include "aoguiloader.h"

DRServerInfoEditor::DRServerInfoEditor(QWidget *parent)
    : QDialog(parent)
{
  setModal(true);

  AOGuiLoader l_loader;
  l_loader.load_from_file(":/src/drserverinfoeditor.ui", this);

  ui_name = AO_GUI_WIDGET(QLineEdit, "name");
  ui_description = AO_GUI_WIDGET(QPlainTextEdit, "description");
  ui_address = AO_GUI_WIDGET(QLineEdit, "address");
  ui_port = AO_GUI_WIDGET(QSpinBox, "port");
  ui_button_box = AO_GUI_WIDGET(QDialogButtonBox, "button_box");

  connect(ui_button_box->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(clear_server_info()));
  connect(ui_button_box, SIGNAL(accepted()), this, SLOT(accept()));
  connect(ui_button_box, SIGNAL(rejected()), this, SLOT(reject()));
}

DRServerInfoEditor::~DRServerInfoEditor()
{}

DRServerInfo DRServerInfoEditor::get_server_info()
{
  DRServerInfo l_server_info;

  l_server_info.name = ui_name->text();
  if (l_server_info.name.isEmpty())
    l_server_info.name = ui_name->placeholderText();
  l_server_info.description = ui_description->toPlainText();
  l_server_info.address = ui_address->text();
  l_server_info.port = ui_port->value();

  return l_server_info;
}

void DRServerInfoEditor::set_server_info(DRServerInfo p_server_info)
{
  ui_name->setText(p_server_info.name);
  ui_description->setPlainText(p_server_info.description);
  ui_address->setText(p_server_info.address);
  ui_port->setValue(p_server_info.port);
}

void DRServerInfoEditor::clear_server_info()
{
  ui_name->setText(nullptr);
  ui_description->setPlainText(nullptr);
  ui_address->setText(nullptr);
  ui_port->setValue(ui_port->minimum());
}
