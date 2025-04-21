#ifndef DRTHEME_H
#define DRTHEME_H

#include "qjsonobject.h"
#include <AOApplication.h>
#include <qjsondocument.h>
#include <qstring.h>



class DRTheme
{
public:
  DRTheme(AOApplication *p_ao_app);

  QString m_currentThemeString;
  QJsonDocument m_currentThemeDocument;
  QJsonObject m_currentThemeObject;

  void InitTheme();

  QString LoadFileString(QString p_path);
  void LoadEffects();
  void LoadWtce();
  void LoadShouts();

  void setup_layers();
  void setup_free_blocks();

  bool read_config_bool(QString p_setting_name);
  int read_config_int(QString p_setting_name);
  QVector<QStringList> get_highlight_characters();
  pos_size_type get_element_dimensions(QString p_identifier, QString p_scene);


  QString get_widget_image(QString p_identifier, QString p_fallback, QString p_scene);
  QColor get_widget_settings_color(QString p_identifier, QString p_scene, QString p_type, QString ini_fallback);
  QPoint get_widget_settings_spacing(QString p_identifier, QString p_scene, QString ini_fallback);
  int get_widget_settings_int(QString p_identifier, QString p_scene, QString p_setting);
  bool get_widget_settings_bool(QString p_identifier, QString p_scene, QString p_setting);


  //Font Functions
  bool get_widget_font_bool(QString p_identifier, QString p_scene, QString p_param, QString p_type);
  QColor get_widget_font_color(QString p_identifier, QString p_scene, QString p_type);

  int get_music_name_speed();

  bool m_jsonLoaded = false;

  QString m_themePath = "";

  QMap<DR::Color, DR::ColorInfo> get_chat_colors();

  QString get_sfx_file(QString p_identifier);

  //Effects
  QStringList get_effect(int index);
  bool getEffectPairOffset(int index);
  int get_effects_count();

  //Shouts
  QString get_shout(int index);
  int get_shouts_count();

  //WTCE
  QString get_wtce(int index);
  int get_wtce_count();

  QString get_free_block(int index);
  int get_free_block_count();

  QVector<QStringList> widget_layers = {};


  //TABS
  QStringList get_tab_names();
  QStringList get_tab_widgets(QString p_tab_name);
  QStringList get_tab_widgets_disable(QString p_tab_name);

  QString get_widget_font_string_setting(QString p_identifier, QString p_param, QString p_scene, QString fallback);
private:
  AOApplication *ao_app = nullptr;
  QJsonObject *get_font_json_object(QString p_identifier, QString p_scene);
  QVector<QStringList> effects = {};
  QVector<QStringList> shouts = {};
  QVector<QStringList> wtce = {};
  QVector<QString> free_blocks = {};
  int effect_count = 0;
  int shouts_count = 0;
  int wtce_count = 0;
  int free_block_count = 0;
};

#endif // DRTHEME_H
