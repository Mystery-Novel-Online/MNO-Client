#ifndef RP_LABEL_H
#define RP_LABEL_H

class AOApplication;

#include <QLabel>

class RPLabel : public QLabel
{
public:
  RPLabel(QWidget *parent, AOApplication *p_ao_app);
  RPLabel(const QString& name, QWidget *parent);

  void set_image(QString p_image);

  void themeRefresh();

private:
  AOApplication *ao_app = nullptr;
  QString m_Friendly = "";
};

#endif // RP_LABEL_H
