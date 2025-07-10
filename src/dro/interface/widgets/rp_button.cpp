#include "rp_button.h"

#include "aoapplication.h"
#include "commondefs.h"
#include "debug_functions.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/theme.h"
#include "drtheme.h"
#include "theme.h"

#include <QDebug>

RPButton::RPButton(QWidget *parent) : QPushButton(parent), m_app(AOApplication::getInstance()) { }

RPButton::RPButton(const QString& name, QWidget *parent) : QPushButton(parent), m_app(AOApplication::getInstance())
{
  set_theme_image(name, "", "courtroom", name);
}

RPButton::RPButton(const QString &name, const QString &image, QWidget *parent) : QPushButton(parent), m_app(AOApplication::getInstance())
{
  set_theme_image(name, image, "courtroom", name);
}

RPButton::RPButton(const QString &name, const QString &image, const QString &fallback, QWidget *parent) : QPushButton(parent), m_app(AOApplication::getInstance())
{
  set_theme_image(name, image, "courtroom", fallback);
}

QString RPButton::get_image()
{
  return m_image;
}

bool RPButton::has_image()
{
  return (!m_image.isEmpty());
}

void RPButton::set_image(QString p_image)
{
  QString path = m_app->find_theme_asset_path(p_image);
  if(m_image == path) return;
  m_image_stem = p_image;
  m_image = path;

  // Get the path of the found image without the extension
  const QString l_image_name = p_image.left(p_image.lastIndexOf(QChar('.')));
  QString l_hover_image = m_app->find_theme_asset_path(l_image_name + "_hover.png");
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

void RPButton::set_theme_image(QString widgetName, QString p_image, QString scene, QString fallbackText)
{
  m_fallbackImage = p_image;
  m_friendlyName = widgetName;
  m_targetScene = scene;
  set_image(m_app->current_theme->get_widget_image(widgetName, p_image, scene));
  if (get_image().isEmpty()) setText(fallbackText);
  else setText("");
  m_fallbackText = fallbackText;
}

void RPButton::set_theme_image()
{
  set_image(m_app->current_theme->get_widget_image(m_friendlyName, m_fallbackImage, m_targetScene));
  if (get_image().isEmpty()) setText(m_fallbackText);
  else setText("");
}

void RPButton::set_image_and_text(QString p_image, QString p_text)
{
  m_text = p_text;
  set_image(p_image);
}

void RPButton::refresh_image()
{
  set_image(m_image_stem);
}

void RPButton::refresh_position()
{
  dro::system::theme::applyDimensions(this, m_friendlyName, SceneType_Courtroom);
}
