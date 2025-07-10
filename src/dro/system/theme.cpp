#include "theme.h"
#include "aoapplication.h"
#include "drtheme.h"
#include "commondefs.h"
#include "modules/theme/thememanager.h"

#include <QWidget>

QMap<ThemeSceneType, QString> LEGACY_DESIGN_INIS =
{
  {SceneType_Courtroom, COURTROOM_DESIGN_INI},
  {SceneType_ServerSelect, LOBBY_DESIGN_INI},
  {SceneType_Replay, REPLAY_DESIGN_INI},
  {SceneType_Viewport, VIEWPORT_DESIGN_INI}
};

namespace dro::system::theme
{
  void applyDimensions(QWidget *widget, QString identifier, ThemeSceneType scene)
  {
    pos_size_type dimensions = getDimensions(identifier, scene);

    if (dimensions.width < 0 || dimensions.height < 0)
    {
      qDebug() << "W: could not find" << identifier << "in " << scene;
      dimensions = {0, 0, 0, 0};
    }

    widget->move(dimensions.x, dimensions.y);
    widget->resize(dimensions.width, dimensions.height);
  }

  pos_size_type getDimensions(QString identifier, ThemeSceneType scene)
  {
    LEGACY_DESIGN_INIS.contains(scene);

    if(AOApplication::getInstance()->current_theme->m_jsonLoaded)
    {
      pos_size_type json_pos = ThemeManager::get().mCurrentThemeReader.GetWidgetTransform(scene, identifier);
      if(json_pos.width != -1) return json_pos;
    }

    pos_size_type return_value = {0, 0, -1, -1};

    if(!LEGACY_DESIGN_INIS.contains(scene)) return return_value;

    QString f_result = AOApplication::getInstance()->read_theme_ini(identifier, LEGACY_DESIGN_INIS[scene]);

    if (f_result.isEmpty()) return return_value;

    QStringList sub_line_elements = f_result.split(",");

    if (sub_line_elements.size() < 4) return return_value;

    return_value.x = sub_line_elements.at(0).toInt();
    return_value.y = sub_line_elements.at(1).toInt();
    return_value.width = sub_line_elements.at(2).toInt();
    return_value.height = sub_line_elements.at(3).toInt();

    return return_value;
  }

}
