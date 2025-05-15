#ifndef RP_BUTTON_H
#define RP_BUTTON_H

class AOApplication;

#include <QPushButton>

class RPButton : public QPushButton
{
  Q_OBJECT

public:
  RPButton(QWidget *parent = nullptr);
  RPButton(const QString& name, QWidget *parent = nullptr);
  RPButton(const QString& name, const QString& image, QWidget *parent = nullptr);
  RPButton(const QString& name, const QString& image, const QString& fallback, QWidget *parent = nullptr);

  QString get_image();
  bool has_image();
  void set_image(QString p_image);
  void set_theme_image(QString widgetName, QString p_image, QString scene, QString fallbackText);
  void set_theme_image();
  void set_image_and_text(QString p_image, QString p_text);
  void refresh_image();
  void refresh_position();

private:
  AOApplication *m_app = nullptr;
  QString m_image;
  QString m_image_stem;
  QString m_text;

  QString m_friendlyName = "widget";
  QString m_fallbackImage = "";
  QString m_fallbackText = "";
  QString m_targetScene = "courtroom";
};

#endif // RP_BUTTON_H
