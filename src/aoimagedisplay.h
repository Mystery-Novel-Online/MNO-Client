#ifndef AOIMAGEDISPLAY_H
#define AOIMAGEDISPLAY_H

class AOApplication;

#include "aopixmap.h"

#include <QLabel>

class AOImageDisplay : public QLabel
{
public:
  AOImageDisplay(QWidget *parent, AOApplication *p_ao_app);

  QString get_image();
  void set_image(QString p_image, bool t_forceRefresh = true);
  void refreshImage();
  void set_theme_image(QString p_image);
  void set_chatbox_image(QString p_chatbox_name, bool p_is_self);

  void SetImageBase(QString l_path, int l_level);

  void UpdateAlpha(int l_AlphaLevel)
  {
    if(m_alphaLevel == l_AlphaLevel) return;
    if(m_Pixmap == nullptr)
    {
      m_Pixmap = new AOPixmap();
    };
    m_Pixmap->SetAlphaMask(l_AlphaLevel);
    m_alphaLevel = l_AlphaLevel;
    setPixmap(m_Pixmap->scale(size()));
  };


private:
  AOApplication *ao_app = nullptr;
  AOPixmap *m_Pixmap = nullptr;

  QString m_image;

  int m_alphaLevel = 255;
};

#endif // AOIMAGEDISPLAY_H
