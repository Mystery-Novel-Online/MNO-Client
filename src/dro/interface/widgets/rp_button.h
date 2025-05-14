#ifndef RP_BUTTON_H
#define RP_BUTTON_H

class AOApplication;

#include <QPushButton>

class RPButton : public QPushButton
{
  Q_OBJECT

public:
  RPButton(QWidget *parent, AOApplication *p_ao_app);

  QString get_image();
  bool has_image();
  void set_image(QString p_image);
  void set_theme_image(QString widgetName, QString p_image, QString scene, QString fallbackText);
  void set_theme_image();
  void set_image_and_text(QString p_image, QString p_text);
  void refresh_image();
  void refresh_position();

private:
  AOApplication *ao_app = nullptr;
  QString m_image;
  QString m_image_stem;
  QString m_text;

  QString widget_name;
  QString fallback_image;
  QString scene_type;
  QString fallback_text;
};

#endif // RP_BUTTON_H
