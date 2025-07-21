#include "pch.h"

#include "aoconfig.h"
#include "dro/system/localization.h"
#include "drcharactermovie.h"
#include "drgraphicscene.h"
#include "theme.h"
#include "drtheme.h"
#include "modules/theme/thememanager.h"

#include "dro/param/actor_repository.h"
#include "dro/param/actor/actor_loader.h"
#include "dro/system/text_encoding.h"
#include "dro/interface/menus/emote_menu.h"
#include "dro/interface/courtroom_layout.h"

int s_emotePreviewIndex = -1;
using namespace dro;

void Courtroom::construct_emotes()
{
  ui_emotes = new EmotionSelector(this);

  ui_emote_left = new RPButton("emote_left", "arrow_left.png", "", this);
  ui_emote_right = new RPButton("emote_right", "arrow_right.png", "", this);

  ui_emote_preview = new DRGraphicsView(nullptr);
  ui_emote_preview->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::BypassGraphicsProxyWidget);
  ui_emote_preview->setAttribute(Qt::WA_TransparentForMouseEvents);

  {
    auto *l_scene = ui_emote_preview->scene();

    ui_emote_preview_background = new DRThemeMovie(ao_app);
    l_scene->addItem(ui_emote_preview_background);

    ui_emote_preview_character = new DRCharacterMovie(ao_app);
    l_scene->addItem(ui_emote_preview_character);
  }

  ui_emote_dropdown = new QComboBox(this);
  ui_pos_dropdown = new QComboBox(this);
  ui_pos_dropdown->addItem(system::localization::getText("DEFAULT"));
  ui_pos_dropdown->addItem(system::localization::getText("POS_WIT"), "wit");
  ui_pos_dropdown->addItem(system::localization::getText("POS_DEF"), "def");
  ui_pos_dropdown->addItem(system::localization::getText("POS_PRO"), "pro");
  ui_pos_dropdown->addItem(system::localization::getText("POS_JUD"), "jud");
  ui_pos_dropdown->addItem(system::localization::getText("POS_DEF_ASS"), "hld");
  ui_pos_dropdown->addItem(system::localization::getText("POS_PRO_ASS"), "hlp");
}

void Courtroom::show_emote_tooltip(int p_id, QPoint p_global_pos)
{
  if (!ao_config->emote_preview_enabled())
    return;

  if (s_emotePreviewIndex != -1 || s_emotePreviewIndex == p_id)
    return;
  s_emotePreviewIndex = p_id;
  const int l_real_id = ui_emotes->calculateTrueIndex(p_id);
  const DREmote &l_emote =  ui_emotes->getEmote(l_real_id);
  ui_emote_preview_character->set_mirrored(ui_flip->isChecked());


  QStringList layers;
  for(const EmoteLayer &layer : l_emote.emoteOverlays)
  {
    if(dro::actor::user::layerState(layer.toggleName))
      layers.append(dro::system::encoding::text::EncodePacketContents({layer.spriteName, layer.spriteOrder, QString::number(layer.layerOffset.x()), QString::number(layer.layerOffset.y()), QString::number(layer.layerOffset.width()), QString::number(layer.layerOffset.height()), layer.offsetName}));
  }

  ui_emote_preview_character->processOverlays(dro::system::encoding::text::EncodeBase64(layers), l_emote.character, l_emote.dialog, l_emote.outfitName);
  ui_emote_preview_character->play_idle(l_emote.character, l_emote.dialog);
  ui_emote_preview_character->setVerticalOffset(courtroom::sliders::getValue("vertical_offset"));
  ui_emote_preview_character->start(dro::actor::user::retrieve()->GetScalingMode(), (double)courtroom::sliders::getValue("scale_offset") / 1000.0f);

  QScreen *screen = QApplication::screenAt(p_global_pos);
  if (screen == nullptr)
  {
    return;
  }
  QRect l_screen_geometry = screen->geometry();

  // position below cursor
  const int l_vertical_spacing = 8;
  QPoint l_final_global_pos(p_global_pos.x(), p_global_pos.y() + l_vertical_spacing);

  if (l_screen_geometry.width() < ui_emote_preview->width() + l_final_global_pos.x())
  {
    l_final_global_pos.setX(p_global_pos.x() - ui_emote_preview->width());
  }

  if (l_screen_geometry.height() < ui_emote_preview->height() + l_final_global_pos.y())
  {
    l_final_global_pos.setY(p_global_pos.y() - ui_emote_preview->height() - l_vertical_spacing);
  }

  ui_emote_preview->move(l_final_global_pos);
  ui_emote_preview->show();
}

void Courtroom::hide_emote_tooltip(int p_id)
{
  if (s_emotePreviewIndex == -1 || s_emotePreviewIndex != p_id)
    return;
  s_emotePreviewIndex = -1;
  ui_emote_preview->hide();
  ui_emote_preview_character->set_file_name(nullptr);
  ui_emote_preview_character->stop();
}

void Courtroom::on_emote_preview_toggled(bool p_enabled)
{
  if (!p_enabled)
    hide_emote_tooltip(s_emotePreviewIndex);
}

void Courtroom::on_emote_left_clicked()
{
  ui_emotes->switchPagePrevious();
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_emote_right_clicked()
{
  ui_emotes->switchPageNext();
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_emote_dropdown_changed(int p_index)
{
  ui_emotes->dropdownChanged(p_index);
}
