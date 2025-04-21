#include "courtroom.h"

#include "aoapplication.h"
#include "aobutton.h"
#include "aoconfig.h"
#include "aoemotebutton.h"
#include "modules/managers/localization_manager.h"
#include "aoimagedisplay.h"
#include "commondefs.h"
#include "drcharactermovie.h"
#include "drgraphicscene.h"
#include "theme.h"
#include "drtheme.h"
#include "modules/theme/thememanager.h"
#include <QCheckBox>
#include <QComboBox>
#include <QFuture>
#include "modules/managers/emotion_manager.h"
#include "modules/managers/character_manager.h"
#include <QLineEdit>
#include <QScreen>
#include <QtMath>
#include <QtConcurrent/QtConcurrent>
#include <modules/theme/widgets/droemotebuttons.h>



void Courtroom::construct_emotes()
{
  ui_emotes = new DROEmoteButtons(this);
  EmotionManager::get().setEmotesPanel(ui_emotes);

  ui_emote_left = setupButtonWidget("emote_left", "arrow_left.png", "");
  ui_emote_right = setupButtonWidget("emote_right", "arrow_right.png", "");

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
  ui_pos_dropdown->addItem(LocalizationManager::get().getLocalizationText("DEFAULT"));
  ui_pos_dropdown->addItem(LocalizationManager::get().getLocalizationText("POS_WIT"), "wit");
  ui_pos_dropdown->addItem(LocalizationManager::get().getLocalizationText("POS_DEF"), "def");
  ui_pos_dropdown->addItem(LocalizationManager::get().getLocalizationText("POS_PRO"), "pro");
  ui_pos_dropdown->addItem(LocalizationManager::get().getLocalizationText("POS_JUD"), "jud");
  ui_pos_dropdown->addItem(LocalizationManager::get().getLocalizationText("POS_DEF_ASS"), "hld");
  ui_pos_dropdown->addItem(LocalizationManager::get().getLocalizationText("POS_PRO_ASS"), "hlp");
}

void Courtroom::construct_emote_page_layout()
{
  // delete previous buttons
  while (!EmotionManager::get().wEmoteList.isEmpty())
    delete EmotionManager::get().wEmoteList.takeLast();

  // resize and move
  set_size_and_pos(ui_emotes, "emotes", COURTROOM_DESIGN_INI, ao_app);

  QPoint f_spacing = ao_app->current_theme->get_widget_settings_spacing("emotes", "courtroom", "emote_button_spacing");

  int buttonResize = (int)((float)40 * ThemeManager::get().getResize());
  const int button_width = buttonResize;
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = buttonResize;
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  emote_columns = ((ui_emotes->width() - button_width) / (x_spacing + button_width)) + 1;
  emote_rows = ((ui_emotes->height() - button_height) / (y_spacing + button_height)) + 1;

  EmotionManager::get().mEmotePageMax = qMax(1, emote_columns * emote_rows);
  for (int n = 0; n < EmotionManager::get().mEmotePageMax; ++n)
  {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    AOEmoteButton *f_emote = new AOEmoteButton(ui_emotes, ao_app, x_pos, y_pos);

    EmotionManager::get().wEmoteList.append(f_emote);

    f_emote->set_emote_number(n);

    connect(f_emote, SIGNAL(emote_clicked(int)), this, SLOT(on_emote_clicked(int)));
    connect(f_emote, SIGNAL(tooltip_requested(int, QPoint)), this, SLOT(show_emote_tooltip(int, QPoint)));
    connect(f_emote, SIGNAL(mouse_left(int)), this, SLOT(hide_emote_tooltip(int)));

    ++x_mod_count;

    if (x_mod_count == emote_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  EmotionManager::get().refreshEmotePage(true);
}

void Courtroom::select_emote(int p_id)
{
  EmotionManager::get().execSelectEmote(p_id);
  if (!SceneManager::get().pConfigAO->sticky_sfx_enabled())
    select_default_sfx();

  ui_emote_dropdown->setCurrentIndex(EmotionManager::get().mEmoteID);

  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_emote_clicked(int p_id)
{
  select_emote(p_id + EmotionManager::get().mEmotePageMax * EmotionManager::get().mCurrentEmotePage);
}

void Courtroom::show_emote_tooltip(int p_id, QPoint p_global_pos)
{
  if (!ao_config->emote_preview_enabled())
    return;

  if (EmotionManager::get().mEmotePreviewID != -1 || EmotionManager::get().mEmotePreviewID == p_id)
    return;
  EmotionManager::get().mEmotePreviewID = p_id;
  const int l_real_id = p_id + EmotionManager::get().mEmotePageMax * EmotionManager::get().mCurrentEmotePage;
  const DREmote &l_emote =  EmotionManager::get().getEmote(l_real_id);
  ui_emote_preview_character->set_mirrored(ui_flip->isChecked());
  ui_emote_preview_character->play_idle(l_emote.character, l_emote.dialog);

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
  if (EmotionManager::get().mEmotePreviewID == -1 || EmotionManager::get().mEmotePreviewID != p_id)
    return;
  EmotionManager::get().mEmotePreviewID = -1;
  ui_emote_preview->hide();
  ui_emote_preview_character->set_file_name(nullptr);
  ui_emote_preview_character->stop();
}

void Courtroom::on_emote_preview_toggled(bool p_enabled)
{
  if (!p_enabled)
    hide_emote_tooltip(EmotionManager::get().mEmotePreviewID);
}

void Courtroom::on_emote_left_clicked()
{
  EmotionManager::get().execEmotePagePrevious();
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_emote_right_clicked()
{
  EmotionManager::get().execEmotePageNext();
  ui_ic_chat_message_field->setFocus();
}

void Courtroom::on_emote_dropdown_changed(int p_index)
{
  select_emote(p_index);
}
