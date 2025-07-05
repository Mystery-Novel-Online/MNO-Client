#include "courtroom.h"

#include "aoapplication.h"
#include "commondefs.h"
#include "dro/fs/fs_reading.h"
#include "drtheme.h"

#include <QCheckBox>
#include <QColor>
#include "dro/system/localization.h"
#include "modules/managers/character_manager.h"
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

#include "dro/fs/fs_reading.h"
#include "dro/system/audio.h"

#include <optional>

std::optional<DRSfx> Courtroom::current_sfx()
{
  QListWidgetItem *l_item = ui_sfx_list->currentItem();
  if (l_item == nullptr)
    return std::nullopt;
  return m_sfx_list.at(l_item->data(Qt::UserRole).toInt());
}

QString Courtroom::current_sfx_file()
{
  const QString l_current_emote_file = ui_emotes->getSelectedEmote().sound_file;
  const std::optional<DRSfx> l_optional_sfx = current_sfx();
  if (!l_optional_sfx.has_value())
    return l_current_emote_file;
  const QString l_file = l_optional_sfx.value().file;
  return l_file == m_sfx_default_file ? l_current_emote_file : l_file;
}

void Courtroom::load_sfx_list_theme()
{

  m_sfx_color_found = ao_app->current_theme->get_widget_settings_color("sfx_list", "courtroom", "found_sfx", "found_song_color");
  m_sfx_color_missing = ao_app->current_theme->get_widget_settings_color("sfx_list", "courtroom", "missing_sfx", "missing_song_color");


  m_animListIdle = ao_app->current_theme->get_widget_settings_color("chara_animations", "courtroom", "unselected_anim", "unselected_anim_color");
  m_animListSelected = ao_app->current_theme->get_widget_settings_color("chara_animations", "courtroom", "selected_anim", "selected_anim_color");
  m_animListHeader = ao_app->current_theme->get_widget_settings_color("chara_animations", "courtroom", "category_header", "category_header_color");


  update_all_sfx_item_color();
}

void Courtroom::load_current_character_sfx_list()
{
  QSignalBlocker l_blocker(ui_sfx_list);
  ui_sfx_list->clear();

  // items
  m_sfx_list.clear();
  m_sfx_list.append(DRSfx(dro::system::localization::getText("DEFAULT"), m_sfx_default_file));
  m_sfx_list.append(DRSfx(dro::system::localization::getText("SFX_SILENCE"), nullptr));

  const QStringList l_sfx_list = ao_app->get_sfx_list();
  for (const QString &i_sfx_line : l_sfx_list)
  {
    const QStringList l_sfx_entry = i_sfx_line.split("=", DR::SplitBehavior::SkipEmptyParts);

    const QString l_name = l_sfx_entry.at(l_sfx_entry.size() - 1).trimmed();
    const QString l_file = QString(l_sfx_entry.size() >= 2 ? l_sfx_entry.at(0) : nullptr).trimmed();
    const bool l_is_found = !ao_app->find_asset_path({ao_app->get_sfx_noext_path(l_file)}, FS::Formats::SupportedAudio()).isEmpty();
    m_sfx_list.append(DRSfx(l_name, l_file, l_is_found));
  }

  // create items
  QList<QListWidgetItem *> l_item_list;
  for (int i = 0; i < m_sfx_list.length(); ++i)
  {
    const DRSfx &i_sfx = m_sfx_list.at(i);
    QListWidgetItem *l_item = new QListWidgetItem(ui_sfx_list);
    l_item_list.append(l_item);
    l_item->setText(i_sfx.name);
    l_item->setData(Qt::UserRole, i);
    set_sfx_item_color(l_item);
  }

  filter_sfx_list();
}

void Courtroom::filter_sfx_list(QString p_filter)
{
  filter_list_widget(ui_sfx_list, p_filter);
}

void Courtroom::filter_sfx_list()
{
  filter_sfx_list(ui_sfx_search->text());
}

void Courtroom::setAnimItemColor(QListWidgetItem *item)
{
  if (item == nullptr) return;
  QColor l_color = m_animListIdle;

  if(item->text().startsWith("[") && item->text().endsWith("]"))
  {
    l_color = m_animListHeader;
  }
  else if (item == animList->currentItem())
  {
    l_color = m_animListSelected;
  }

  item->setBackground(l_color);
}

void Courtroom::select_default_sfx()
{
  if (ui_sfx_list->count() == 0)
    return;
  ui_sfx_list->setCurrentRow(0);
}

void Courtroom::clear_sfx_selection()
{
  ui_sfx_list->setCurrentRow(-1);
}

void Courtroom::set_sfx_item_color(QListWidgetItem *p_item)
{
  if (p_item == nullptr)
    return;
  const bool l_is_found = m_sfx_list.at(p_item->data(Qt::UserRole).toInt()).is_found;
  QColor l_color = l_is_found ? m_sfx_color_found : m_sfx_color_missing;

  if (p_item == ui_sfx_list->currentItem())
  {
    // Calculate the amount of lightness it would take to light up the row. We
    // also limit it to 1.0, as giving lightness values above 1.0 to QColor does
    // nothing. +0.4 is just an arbitrarily chosen number.
    const double l_final_lightness = qMin(1.0, l_color.lightnessF() + 0.4);

    // This is just the reverse of the above, basically. We set the colour, and we
    // set the brush to have that colour.
    l_color.setHslF(l_color.hueF(), l_color.saturationF(), l_final_lightness);
  }

  p_item->setBackground(l_color);
}

void Courtroom::onAnimListItemChanged(QListWidgetItem *current_item, QListWidgetItem *previous_item)
{
  for (int i = 0; i < animList->count(); ++i)
  {
    setAnimItemColor(animList->item(i));
  }
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::update_all_sfx_item_color()
{
  for (int i = 0; i < m_sfx_list.length(); ++i)
    set_sfx_item_color(ui_sfx_list->item(i));
}

void Courtroom::on_sfx_list_current_item_changed(QListWidgetItem *p_current_item, QListWidgetItem *p_previous_item)
{
  set_sfx_item_color(p_current_item);
  set_sfx_item_color(p_previous_item);
  ui_pre->setChecked(ui_pre->isChecked() || current_sfx().has_value());
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_sfx_list_context_menu_requested(QPoint p_point)
{
  const QPoint l_global_point = ui_sfx_list->viewport()->mapToGlobal(p_point);
  ui_sfx_menu->popup(l_global_point);
}

void Courtroom::on_sfx_menu_preview_triggered()
{
  audio::effect::Play(current_sfx_file().toStdString());
}

void Courtroom::on_sfx_menu_insert_file_name_triggered()
{
  ui_ooc_chat_message->insert(current_sfx_file());
  ui_ooc_chat_message->setFocus();
}

void Courtroom::on_sfx_menu_insert_caption_triggered()
{
  const std::optional<DRSfx> l_sfx = current_sfx();
  if (l_sfx.has_value())
  {
    QString l_caption = l_sfx->name;
    static const QRegularExpression l_regex("\"(.+)\"");
    if (const auto l_match = l_regex.match(l_caption); l_match.hasMatch())
    {
      l_caption = l_match.captured(1);
    }
    ui_ic_chat_message_field->insert(l_caption);
  }
}
