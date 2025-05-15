#ifndef RP_LABEL_H
#define RP_LABEL_H

class AOApplication;

#include <QLabel>

class RPLabel : public QLabel
{
public:
  RPLabel(QWidget *parent, AOApplication *p_ao_app);

  void set_image(QString p_image);

private:
  AOApplication *ao_app = nullptr;
};

#endif // RP_LABEL_H
