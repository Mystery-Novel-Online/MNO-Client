#include "aobutton.h"

#include "aoapplication.h"
#include "commondefs.h"
#include "debug_functions.h"
#include "dro/fs/file_utils.h"
#include "drtheme.h"
#include "theme.h"

#include <QDebug>

AOButton::AOButton(QWidget *parent, AOApplication *p_ao_app)
    : QPushButton(parent)
{
  ao_app = p_ao_app;
}

QString AOButton::get_image()
{
  return m_image;
}

bool AOButton::has_image()
{
  return (!m_image.isEmpty());
}

void AOButton::set_image(QString p_image)
{
  QString path = ao_app->find_theme_asset_path(p_image);
  if(m_image == path) return;
  m_image_stem = p_image;
  m_image = path;

  // Get the path of the found image without the extension
  const QString l_image_name = p_image.left(p_image.lastIndexOf(QChar('.')));
  QString l_hover_image = ao_app->find_theme_asset_path(l_image_name + "_hover.png");
  if (l_hover_image.isEmpty())
  {
    l_hover_image = m_image;
  }

  setStyleSheet("QPushButton {border-image:url(\"" + m_image +
                "\");}"
                "QPushButton:hover {border-image:url(\"" +
                l_hover_image + "\");}");
  setText(m_image.isEmpty() ? m_text : nullptr);
}

void AOButton::set_theme_image(QString widgetName, QString p_image, QString scene, QString fallbackText)
{
  fallback_image = p_image;
  widget_name = widgetName;
  scene_type = scene;
  set_image(ao_app->current_theme->get_widget_image(widgetName, p_image, scene));
  if (get_image().isEmpty()) setText(fallbackText);
  else setText("");
  fallback_text = fallbackText;
}

void AOButton::set_theme_image()
{
  set_image(ao_app->current_theme->get_widget_image(widget_name, fallback_image, scene_type));
  if (get_image().isEmpty()) setText(fallback_text);
  else setText("");
}

void AOButton::set_image_and_text(QString p_image, QString p_text)
{
  m_text = p_text;
  set_image(p_image);
}

void AOButton::refresh_image()
{
  set_image(m_image_stem);
}

void AOButton::refresh_position()
{
  set_size_and_pos(this, widget_name, COURTROOM_DESIGN_INI, ao_app);
}
