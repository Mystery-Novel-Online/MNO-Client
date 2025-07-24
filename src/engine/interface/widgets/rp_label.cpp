#include "theme.h"
#include "engine/system/theme.h"

RPLabel::RPLabel(QWidget *parent, AOApplication *p_ao_app)
    : QLabel(parent)
{
  ao_app = p_ao_app;
}

RPLabel::RPLabel(const QString &name, QWidget *parent)
    : QLabel(parent)
{
  m_Friendly = name;
  ao_app = AOApplication::getInstance();
}

void RPLabel::set_image(QString p_image)
{
  const QString l_image_path = ao_app->find_theme_asset_path(p_image);
  setStyleSheet("border-image:url(\"" + l_image_path + "\")");
}

void RPLabel::themeRefresh()
{
  engine::system::theme::applyDimensions(this, m_Friendly, SceneType_Courtroom);
}
