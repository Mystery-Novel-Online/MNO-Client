#include "dro/interface/widgets/rp_label.h"

#include "aoapplication.h"

RPLabel::RPLabel(QWidget *parent, AOApplication *p_ao_app)
    : QLabel(parent)
{
  ao_app = p_ao_app;
}

void RPLabel::set_image(QString p_image)
{
  const QString l_image_path = ao_app->find_theme_asset_path(p_image);
  setStyleSheet("border-image:url(\"" + l_image_path + "\")");
}
