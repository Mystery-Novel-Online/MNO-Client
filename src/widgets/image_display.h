#ifndef IMAGE_DISPLAY_H
#define IMAGE_DISPLAY_H

class AOApplication;

#include <QLabel>

class AOImageDisplay : public QLabel
{
public:
  AOImageDisplay(QWidget *parent, AOApplication *p_ao_app);
  AOImageDisplay(QWidget *parent);

  QString get_image();
  void setAlpha(QString alphaName) {m_alphaName = alphaName;};
  void set_image(QString p_image);
  void refreshImage();
  void set_theme_image(QString p_image);
  void set_chatbox_image(QString p_chatbox_name, bool p_is_self);

private:
  AOApplication *ao_app = nullptr;

  QString m_image;
  QString m_alphaName = "";
};

#endif // IMAGE_DISPLAY_H
