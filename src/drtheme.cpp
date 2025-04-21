#include "drtheme.h"
#include "commondefs.h"
#include "file_functions.h"
#include "qjsonobject.h"
#include "aoconfig.h"
#include <QJsonArray>

#include "modules/theme/thememanager.h"

#include <qcolor.h>
#include <qfile.h>

DRTheme::DRTheme(AOApplication *p_ao_app)
{
  ao_app = p_ao_app;
}

void DRTheme::InitTheme()
{
  ThemeManager::get().loadTheme(ao_app->getCurrentTheme());
  ThemeManager::get().LoadGamemode(ao_app->getCurrentGamemode());
  ThemeManager::get().mCurrentThemeReader.SetTime(ao_app->getCurrentTime());
  const QString l_json_path = ao_app->find_theme_asset_path(THEME_JSON);
  m_themePath = ao_app->find_current_theme_path();

  if(!file_exists(l_json_path))
  {
    m_currentThemeString = "";
    m_jsonLoaded = false;
  }
  else
  {
    QFile json_file(l_json_path); json_file.open(QIODevice::ReadOnly | QIODevice::Text);
    m_currentThemeString = json_file.readAll();
    json_file.close();

    m_currentThemeDocument = QJsonDocument::fromJson(m_currentThemeString.toUtf8());
    m_currentThemeObject = m_currentThemeDocument.object();

    m_jsonLoaded = true;

    setup_free_blocks();
    setup_layers();
  }
  LoadEffects();
  LoadWtce();
  LoadShouts();

}

void DRTheme::setup_layers()
{
  widget_layers = ThemeManager::get().mCurrentThemeReader.getLayers();
  return;
}

void DRTheme::setup_free_blocks()
{
  free_blocks = {};
  //free_blocks = ThemeManager::get().mCurrentThemeReader->;
  free_block_count = 0;
  QJsonValue free_blocks_array_value = m_currentThemeObject.value(QString("free_blocks"));
  QJsonArray free_blocks_array = free_blocks_array_value.toArray();

  for(QJsonValueRef i : free_blocks_array)
  {
    QJsonObject free_block_object = i.toObject();
    QString free_block_name = free_block_object["name"].toString();

    if(!free_block_name.isEmpty())
    {
      free_blocks.append(free_block_name);
      free_block_count += 1;
    }
  }

  return;
}

QString DRTheme::LoadFileString(QString p_path)
{
  if(!file_exists(p_path))
  {
    return "";
  }

  QFile json_file(p_path);
  json_file.open(QIODevice::ReadOnly | QIODevice::Text);
  QString return_string = json_file.readAll(); json_file.close();
  return return_string;
}

void DRTheme::LoadEffects()
{
  const QString l_effects_json_path = ao_app->get_base_path() + "effects/default/effects.json";

  effects = {};
  effect_count = 0;

  QString json_string = LoadFileString(l_effects_json_path);
  if(json_string.isEmpty())
  {
    return;
  }

  QJsonDocument l_document = QJsonDocument::fromJson(json_string.toUtf8());
  QJsonArray l_array = l_document.array();
  for(QJsonValueRef effect : l_array)
  {
    QJsonObject effect_object = effect.toObject();
    effects.append({effect_object["effect_name"].toString(), effect_object["loop"].toBool() ? "0" : "1", QString::number(effect_object["id"].toInt()), effect_object["ignore_pair"].toBool() ? "0" : "1"});
    effect_count += 1;
  }

}

void DRTheme::LoadWtce()
{
  const QString l_wtce_json_path = ao_app->get_base_path() + "shouts/default/wtce.json";

  wtce = {};
  wtce_count = 0;

  QString json_string = LoadFileString(l_wtce_json_path);
  if(json_string.isEmpty())
  {
    return;
  }

  QJsonDocument l_document = QJsonDocument::fromJson(json_string.toUtf8());
  QJsonArray l_array = l_document.array();
  for(QJsonValueRef wtce_value : l_array)
  {
    QJsonObject wtce_object = wtce_value.toObject();
    QString wtce_name = wtce_object["wtce_name"].toString();

    if(!wtce_name.isEmpty())
    {
      wtce.append({wtce_name, QString::number(wtce_object["id"].toInt())});
      wtce_count += 1;
    }
  }
}

void DRTheme::LoadShouts()
{
  const QString l_shouts_json_path = ao_app->get_base_path() + "shouts/default/shouts.json";

  shouts = {};
  shouts_count = 0;

  QString json_string = LoadFileString(l_shouts_json_path);
  if(json_string.isEmpty())
  {
    return;
  }

  QJsonDocument l_document = QJsonDocument::fromJson(json_string.toUtf8());
  QJsonArray l_array = l_document.array();
  for(QJsonValueRef shout : l_array)
  {
    QJsonObject shout_object = shout.toObject();
    QString shout_name = shout_object["shout_name"].toString();
    if(!shout_name.isEmpty())
    {
      shouts.append({shout_name, QString::number(shout_object["id"].toInt())});
      shouts_count += 1;
    }
  }

}

bool DRTheme::read_config_bool(QString p_setting_name)
{
  if(!m_jsonLoaded)
  {
    return ao_app->read_theme_ini_bool(p_setting_name, COURTROOM_CONFIG_INI);
  }

  return ThemeManager::get().getConfigBool(p_setting_name);
}

int DRTheme::read_config_int(QString p_setting_name)
{
  int lReturnValue = 0;
  if(!m_jsonLoaded)
  {
    int lReturnValue = ao_app->read_theme_ini_int(p_setting_name, COURTROOM_CONFIG_INI);
    if(lReturnValue < 0) lReturnValue = 0;
    return lReturnValue;
  }

  lReturnValue = ThemeManager::get().mCurrentThemeReader.getTimerNumber();

  if(lReturnValue < 0) lReturnValue = 0;
  return lReturnValue;
}

QVector<QStringList>DRTheme::get_highlight_characters()
{
  return ThemeManager::get().mCurrentThemeReader.getHighlights();
}

pos_size_type DRTheme::get_element_dimensions(QString p_identifier, QString p_scene)
{
  if(p_scene == "courtroom")
  {
    return ThemeManager::get().mCurrentThemeReader.getWidgetPosition(COURTROOM, p_identifier);
  }
  else
  {
    return ThemeManager::get().mCurrentThemeReader.getWidgetPosition(LOBBY, p_identifier);
  }

}

QString DRTheme::get_widget_image(QString p_identifier, QString p_fallback, QString p_scene)
{
  if(!m_jsonLoaded)
  {
    return p_fallback;
  }

  QJsonValue value = m_currentThemeObject.value(QString(p_scene));
  QJsonObject item = value.toObject();
  QJsonObject element_image = item[p_identifier].toObject();

  if(!element_image.contains("image"))
  {
    return p_fallback;
  }

  return element_image["image"].toString();

};


QString DRTheme::get_widget_font_string_setting(QString p_identifier, QString p_param, QString p_scene, QString fallback)
{
  if(!m_jsonLoaded)
  {
    return ao_app->read_theme_ini(fallback, p_scene);
  }

  ThemeSceneType sceneType = COURTROOM;
  if(p_scene == LOBBY_FONTS_INI) sceneType = LOBBY;
  return ThemeManager::get().mCurrentThemeReader.getFont(sceneType, p_identifier).align;
}

bool DRTheme::get_widget_font_bool(QString p_identifier, QString p_scene, QString p_param, QString p_type)
{
  if(!m_jsonLoaded)
  {
    return false;
  }

  QJsonValue value = m_currentThemeObject.value(QString(p_scene));
  QJsonObject item = value.toObject();
  QJsonObject element_font = item[p_identifier].toObject();

  bool l_font_bool = element_font["font"].toObject()[p_type + "_" + p_param].toBool();

  return l_font_bool;

}


QColor DRTheme::get_widget_font_color(QString p_identifier, QString p_scene, QString p_type)
{
  QColor return_value = QColor(0,0,0);
  if(!m_jsonLoaded)
  {
    return return_value;
  }

  QJsonValue value = m_currentThemeObject.value(QString(p_scene));
  QJsonObject item = value.toObject();
  QJsonObject element_font = item[p_identifier].toObject();


  if(!element_font["font"].toObject().contains(p_type + "_color"))
  {
    return return_value;
  }

  return_value = QColor(element_font["font"].toObject()[p_type + "_color"].toString());

  return return_value;

}

int DRTheme::get_widget_settings_int(QString p_identifier, QString p_scene, QString p_setting)
{

  int return_value = 0;
  if(!m_jsonLoaded)
  {
    return return_value;
  }

  QJsonValue value = m_currentThemeObject.value(QString(p_scene));
  QJsonObject item = value.toObject();
  QJsonObject widget_object = item[p_identifier].toObject();


  if(!widget_object["settings"].toObject().contains(p_setting))
  {
    return return_value;
  }

  return_value = widget_object["settings"].toObject()[p_setting].toInt();

  return return_value;

}

bool DRTheme::get_widget_settings_bool(QString p_identifier, QString p_scene, QString p_setting)
{

  bool return_value = false;
  if(!m_jsonLoaded)
  {
    return return_value;
  }

  QJsonValue value = m_currentThemeObject.value(QString(p_scene));
  QJsonObject item = value.toObject();
  QJsonObject widget_object = item[p_identifier].toObject();


  if(!widget_object["settings"].toObject().contains(p_setting))
  {
    return return_value;
  }

  return_value = widget_object["settings"].toObject()[p_setting].toBool();

  return return_value;
}

QColor DRTheme::get_widget_settings_color(QString p_identifier, QString p_scene, QString p_type, QString ini_fallback)
{

  QColor return_value = QColor(0,0,0);
  if(!m_jsonLoaded)
  {
    return ao_app->get_color(ini_fallback, COURTROOM_DESIGN_INI);
  }

  QString l_color = ThemeManager::get().mCurrentThemeReader.getSoundName(p_type + "_color");
  if(l_color.isEmpty())
  {
    return ao_app->get_color(ini_fallback, COURTROOM_DESIGN_INI);
  }

  return_value = QColor(l_color);

  return return_value;

}

QPoint DRTheme::get_widget_settings_spacing(QString p_identifier, QString p_scene, QString ini_fallback)
{
  if(!m_jsonLoaded)
  {
    return ao_app->get_button_spacing(ini_fallback, COURTROOM_DESIGN_INI);
  }

  QVector2D spacing = ThemeManager::get().mCurrentThemeReader.getWidgetSpacing(p_identifier);
  QPoint return_value = QPoint(spacing.x(),spacing.y());
  return return_value;
}

QJsonObject *DRTheme::get_font_json_object(QString p_identifier, QString p_scene)
{
  return nullptr;
}

QMap<DR::Color, DR::ColorInfo> DRTheme::get_chat_colors()
{
  QMap<DR::Color, DR::ColorInfo> color_map = DR::get_default_color_map();

  if(!m_jsonLoaded)
  {
    return color_map;
  }

  QJsonValue value = m_currentThemeObject.value(QString("config"));
  QJsonObject item = value.toObject();
  QJsonArray array = item["colors"].toArray();


  QMap<QString, DR::ColorInfo> color_replacement_map = ThemeManager::get().mCurrentThemeReader.getTextColors();

  for (DR::Color &i_color : color_map.keys())
  {
    DR::ColorInfo &color_info = color_map[i_color];
    const QString lower_name = color_info.name.toLower();
    if (!color_replacement_map.contains(lower_name))
      continue;
    color_info.code = color_replacement_map[lower_name].code;
  }


  return color_map;
}

QString DRTheme::get_sfx_file(QString p_identifier)
{
  if(!m_jsonLoaded) return ao_app->read_theme_ini(p_identifier, COURTROOM_SOUNDS_INI);

  return ThemeManager::get().mCurrentThemeReader.getSoundName(p_identifier);
}

QStringList DRTheme::get_effect(int index)
{
  for(QStringList effect : effects)
  {
    if(effect.at(2) == QString::number(index)) return QStringList{effect.at(0), effect.at(1)};
  }

  return QStringList{"", ""};
}

bool DRTheme::getEffectPairOffset(int index)
{
  for(QStringList effect : effects)
  {
    if(effect.at(2) == QString::number(index)) return effect.at(3) == "1";
  }

  return false;
}

QString DRTheme::get_shout(int index)
{
  for(QStringList shout : shouts)
  {
    if(shout.at(1) == QString::number(index)) return shout.at(0);
  }

  return "";
}

QString DRTheme::get_wtce(int index)
{
  for(QStringList wtce_value : wtce)
  {
    if(wtce_value.at(1) == QString::number(index)) return wtce_value.at(0);
  }

  return "";
}

int DRTheme::get_effects_count()
{
  return effect_count;
}

int DRTheme::get_shouts_count()
{
  return shouts_count;
}

int DRTheme::get_wtce_count()
{
  return wtce_count;
}

QString DRTheme::get_free_block(int index)
{
  if(free_blocks.length() <= index) return "";
  return free_blocks[index];
}

int DRTheme::get_free_block_count()
{
  if(!m_jsonLoaded)
  {
    return ao_app->read_theme_ini_int("free_block_number", COURTROOM_CONFIG_INI);
  }
  return free_block_count;
}


QStringList DRTheme::get_tab_names()
{
  QStringList l_tab_names = {};
  QVector<ThemeTabInfo> l_tabs = ThemeManager::get().mCurrentThemeReader.getTabs();

  for(ThemeTabInfo tab : l_tabs)
  {
    if(!tab.m_Name.isEmpty())
    {
      l_tab_names.append(tab.m_Name);
    }
  }

  return l_tab_names;
}

QStringList DRTheme::get_tab_widgets(QString p_tab_name)
{
  QStringList widget_names = {};

  QVector<ThemeTabInfo> l_tabs = ThemeManager::get().mCurrentThemeReader.getTabs();

  for(ThemeTabInfo tab : l_tabs)
  {

    for(QString widget_name : tab.m_WidgetContents)
    {
      if(!widget_name.isEmpty() && tab.m_Name.toLower() == p_tab_name.toLower())
      {
        widget_names.append(widget_name);
      }
    }
  }
  return widget_names;
}

QStringList DRTheme::get_tab_widgets_disable(QString p_tab_name)
{
  QStringList widget_names = {};

  QVector<ThemeTabInfo> l_tabs = ThemeManager::get().mCurrentThemeReader.getTabs();

  for(ThemeTabInfo tab : l_tabs)
  {

    for(QString widget_name : tab.m_WidgetContents)
    {
      if(!widget_name.isEmpty() && tab.m_Name != p_tab_name.toLower())
      {
        widget_names.append(widget_name);
      }
    }
  }

  return widget_names;
}

int DRTheme::get_music_name_speed()
{
  int speed = 25000;
  if(!m_jsonLoaded)
  {
    speed = ao_app->get_font_property("music_name_speed", COURTROOM_FONTS_INI);
  }
  else
  {
    speed = ThemeManager::get().mCurrentThemeReader.getMusicScrollSpeed();
  }

  return speed;
}
