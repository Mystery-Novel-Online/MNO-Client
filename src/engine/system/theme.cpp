#include "theme.h"

#include "drtheme.h"
#include "modules/theme/thememanager.h"
#include "pch.h"

const RPRect FALLBACK_DIMENSIONS = {0, 0, -1, -1};

static const QStringList CHATBOX_ELEMENTS = {"showname", "ao2_chatbox", "message", "chat_arrow"};
static const QStringList POSITIONAL_ALIGNMENTS = {"", "_left", "_right"};

const QMap<ThemeSceneType, QString> LEGACY_DESIGN_INIS =
{
  {ThemeSceneType::SceneType_Courtroom, COURTROOM_DESIGN_INI},
  {ThemeSceneType::SceneType_ServerSelect, LOBBY_DESIGN_INI},
  {ThemeSceneType::SceneType_Replays, REPLAY_DESIGN_INI},
  {ThemeSceneType::SceneType_Viewport, VIEWPORT_DESIGN_INI}
};

QMap<QString, RPRect> s_PositionalDimensions = {};

namespace engine::system::theme
{
  void applyDimensions(QWidget *widget, const QString &identifier, ThemeSceneType scene, bool allowResize)
  {
    RPRect dimensions = getDimensions(identifier, scene);

    if (dimensions.width < 0 || dimensions.height < 0)
    {
      qDebug() << "W: could not find" << identifier << "in " << (int)scene;
      dimensions = {0, 0, 0, 0};
    }

    widget->move(dimensions.x, dimensions.y);
    if(allowResize)
      widget->resize(dimensions.width, dimensions.height);
  }

  RPRect getDimensions(const QString &identifier, ThemeSceneType scene)
  {
    LEGACY_DESIGN_INIS.contains(scene);

    if(AOApplication::getInstance()->current_theme->m_jsonLoaded)
    {
      RPRect json_pos = ThemeManager::get().mCurrentThemeReader.GetWidgetTransform(scene, identifier);
      if(json_pos.width != -1) return json_pos;
    }

    if(!LEGACY_DESIGN_INIS.contains(scene)) return FALLBACK_DIMENSIONS;

    QString f_result = AOApplication::getInstance()->read_theme_ini(identifier, LEGACY_DESIGN_INIS[scene]);

    if (f_result.isEmpty()) return FALLBACK_DIMENSIONS;

    QStringList sub_line_elements = f_result.split(",");

    if (sub_line_elements.size() < 4) return FALLBACK_DIMENSIONS;

    RPRect return_value;
    return_value.x = sub_line_elements.at(0).toInt();
    return_value.y = sub_line_elements.at(1).toInt();
    return_value.width = sub_line_elements.at(2).toInt();
    return_value.height = sub_line_elements.at(3).toInt();

    return return_value;
  }

  void reloadMetadata()
  {
    s_PositionalDimensions.clear();

    for(QString element : CHATBOX_ELEMENTS)
    {
      for(QString alignment : POSITIONAL_ALIGNMENTS)
      {
        QString fullName = element + alignment;
        RPRect sizing = getDimensions(fullName, ThemeSceneType::SceneType_Courtroom);

        if(sizing.height != 0 && sizing.width != 0)
          s_PositionalDimensions[fullName] = sizing;
      }
    }

  }

  RPRect getPositionalDimensions(const QString &identifier, const QString &alignment)
  {
    QString positionalIdentifier = identifier + "_" + alignment;

    if(s_PositionalDimensions.contains(positionalIdentifier)) return s_PositionalDimensions[positionalIdentifier];
    else if(s_PositionalDimensions.contains(identifier)) return s_PositionalDimensions[identifier];

    return FALLBACK_DIMENSIONS;
  }


}
