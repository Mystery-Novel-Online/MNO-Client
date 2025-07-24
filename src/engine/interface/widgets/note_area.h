#ifndef NOTE_AREA_H
#define NOTE_AREA_H

class AOApplication;
class RPButton;

class QVBoxLayout;

class AONoteArea : public AOImageDisplay
{
  Q_OBJECT

public:
  RPButton *add_button = nullptr;
  QVBoxLayout *m_layout = nullptr;

  AONoteArea(QWidget *p_parent, AOApplication *p_ao_app);

private:
  AOApplication *ao_app = nullptr;

  void set_layout();
};

#endif // NOTE_AREA_H
