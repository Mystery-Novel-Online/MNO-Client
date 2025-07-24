#include "theme.h"

#include "drtheme.h"
#include "modules/theme/thememanager.h"
#include "pch.h"

const pos_size_type FALLBACK_DIMENSIONS = {0, 0, -1, -1};

static const QStringList CHATBOX_ELEMENTS = {"showname", "ao2_chatbox", "message", "chat_arrow"};
static const QStringList POSITIONAL_ALIGNMENTS = {"", "_left", "_right"};

const QMap<ThemeSceneType, QString> LEGACY_DESIGN_INIS =
{
  {SceneType_Courtroom, COURTROOM_DESIGN_INI},
  {SceneType_ServerSelect, LOBBY_DESIGN_INI},
  {SceneType_Replay, REPLAY_DESIGN_INI},
  {SceneType_Viewport, VIEWPORT_DESIGN_INI}
};

QMap<QString, pos_size_type> s_PositionalDimensions = {};

namespace engine::system::theme
{
  void applyDimensions(QWidget *widget, const QString &identifier, ThemeSceneType scene, bool allowResize)
  {
    pos_size_type dimensions = getDimensions(identifier, scene);

    if (dimensions.width < 0 || dimensions.height < 0)
    {
      qDebug() << "W: could not find" << identifier << "in " << scene;
      dimensions = {0, 0, 0, 0};
    }

    widget->move(dimensions.x, dimensions.y);
    if(allowResize)
      widget->resize(dimensions.width, dimensions.height);
  }

  pos_size_type getDimensions(const QString &identifier, ThemeSceneType scene)
  {
    LEGACY_DESIGN_INIS.contains(scene);

    if(AOApplication::getInstance()->current_theme->m_jsonLoaded)
    {
      pos_size_type json_pos = ThemeManager::get().mCurrentThemeReader.GetWidgetTransform(scene, identifier);
      if(json_pos.width != -1) return json_pos;
    }

    if(!LEGACY_DESIGN_INIS.contains(scene)) return FALLBACK_DIMENSIONS;

    QString f_result = AOApplication::getInstance()->read_theme_ini(identifier, LEGACY_DESIGN_INIS[scene]);

    if (f_result.isEmpty()) return FALLBACK_DIMENSIONS;

    QStringList sub_line_elements = f_result.split(",");

    if (sub_line_elements.size() < 4) return FALLBACK_DIMENSIONS;

    pos_size_type return_value;
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
        pos_size_type sizing = getDimensions(fullName, SceneType_Courtroom);

        if(sizing.height != 0 && sizing.width != 0)
          s_PositionalDimensions[fullName] = sizing;
      }
    }

  }

  pos_size_type getPositionalDimensions(const QString &identifier, const QString &alignment)
  {
    QString positionalIdentifier = identifier + "_" + alignment;

    if(s_PositionalDimensions.contains(positionalIdentifier)) return s_PositionalDimensions[positionalIdentifier];
    else if(s_PositionalDimensions.contains(identifier)) return s_PositionalDimensions[identifier];

    return FALLBACK_DIMENSIONS;
  }


}
