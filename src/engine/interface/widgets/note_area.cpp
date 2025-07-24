#include "note_area.h"
#include "pch.h"

#include "note_picker.h"
#include "theme.h"
#include "engine/fs/fs_reading.h"

AONoteArea::AONoteArea(QWidget *p_parent, AOApplication *p_ao_app)
    : AOImageDisplay(p_parent, p_ao_app)
{
  ao_app = p_ao_app;
}

void Courtroom::on_add_button_clicked()
{
  if (ui_note_area->m_layout->count() > 6)
    return;

  AONotePicker *f_notepicker = new AONotePicker(ui_note_area, ao_app);
  RPButton *f_button = new RPButton(f_notepicker);
  RPButton *f_delete = new RPButton(f_notepicker);
  QLineEdit *f_line = new QLineEdit(f_notepicker);
  RPButton *f_hover = new RPButton(f_notepicker);
  QHBoxLayout *f_layout = new QHBoxLayout(f_notepicker);

  f_notepicker->ui_line = f_line;
  f_notepicker->ui_button = f_button;
  f_notepicker->ui_layout = f_layout;
  f_notepicker->ui_delete_button = f_delete;
  f_notepicker->ui_hover = f_hover;
  f_notepicker->setProperty("index", ui_note_area->m_layout->count() - 1);

  f_button->set_image("note_edit.png");
  f_delete->set_image("note_delete.png");
  f_hover->set_image("note_select.png");

  f_line->setReadOnly(true);

  f_layout->setSizeConstraint(QLayout::SetMinAndMaxSize);

  f_layout->addWidget(f_hover);
  f_layout->addWidget(f_line);
  f_layout->addWidget(f_button);
  f_layout->addWidget(f_delete);
  f_notepicker->setLayout(f_layout);
  ui_note_area->m_layout->addWidget(f_notepicker);

  if (contains_add_button)
  {
    ui_note_area->m_layout->removeWidget(ui_note_area->add_button);
    ui_note_area->m_layout->addWidget(ui_note_area->add_button);
    set_note_files();
  }

  set_stylesheet(f_line, "[LINE EDIT]", COURTROOM_STYLESHEETS_CSS, ao_app);

  connect(f_button, SIGNAL(clicked(bool)), this, SLOT(on_set_file_button_clicked()));
  connect(f_delete, SIGNAL(clicked(bool)), this, SLOT(on_delete_button_clicked()));
  connect(f_hover, SIGNAL(clicked(bool)), this, SLOT(on_file_selected()));
}

void Courtroom::set_note_files()
{
  QString filename = FS::Paths::BasePath() + CONFIG_FILESABSTRACT_INI;
  QFile config_file(filename);

  if (!config_file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug() << "Couldn't open" << filename;
    return;
  }

  QTextStream in(&config_file);

  QByteArray t;

  for (int i = 0; i < ui_note_area->m_layout->count() - 1; ++i)
  {
    AONotePicker *f_notepicker = static_cast<AONotePicker *>(ui_note_area->m_layout->itemAt(i)->widget());
    QString f_filestring = f_notepicker->m_file;
    QString f_filename = f_notepicker->ui_line->text();

    t += QString("%1 = %2 = %3\n\n").arg(i).arg(f_filestring).arg(f_filename).toUtf8();
  }

  config_file.close();

  QFile ex(filename);
  if (!ex.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
  {
    qDebug() << "Couldn't open" << filename;
    return;
  }

  ex.write(t);
  ex.close();
}
