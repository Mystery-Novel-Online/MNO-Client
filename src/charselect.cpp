#include "courtroom.h"

#include "aoapplication.h"
#include "aobutton.h"
#include "aocharbutton.h"
#include "aoconfig.h"
#include "aoimagedisplay.h"
#include "commondefs.h"
#include "debug_functions.h"
#include "modules/managers/localization_manager.h"
#include "drpacket.h"
#include "file_functions.h"
#include "hardware_functions.h"
#include "modules/managers/character_manager.h"
#include "qcombobox.h"
#include "theme.h"
#include "drtheme.h"

#include <QDebug>
#include <QSignalMapper>
#include <QtMath>

#include <modules/theme/widgets/characterselectwidget.h>

void Courtroom::construct_char_select()
{
  ui_char_select_background = new AOImageDisplay(this, ao_app);

  ui_char_buttons = new CharacterSelectWidget(ui_char_select_background, ao_app);

  ui_char_button_selector = new AOImageDisplay(ui_char_buttons, ao_app);
  ui_char_button_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_char_button_selector->resize(62, 62);

  ui_back_to_lobby = setupButtonWidget("back_to_lobby", "lobby_return.png", LocalizationManager::get().getLocalizationText("CSS_DISCONNECT"), ui_char_select_background);

  ui_chr_select_left = setupButtonWidget("char_select_left", "arrow_left.png", "", ui_char_select_background);
  ui_chr_select_right = setupButtonWidget("char_select_right", "arrow_right.png", "", ui_char_select_background);

  ui_spectator = setupButtonWidget("spectator", "spectator.png", LocalizationManager::get().getLocalizationText("CSS_SPECTATE"), ui_char_select_background);

  pBtnCharSelectRandom = setupButtonWidget("char_select_random", "char_random.png", LocalizationManager::get().getLocalizationText("CSS_RANDOM"), ui_char_select_background);
  pBtnCharSelectRefresh = setupButtonWidget("char_select_refresh", "char_refresh.png", LocalizationManager::get().getLocalizationText("REFRESH"), ui_char_select_background);

  pCharaSelectSearch = setupLineEditWidget("character_search", LocalizationManager::get().getLocalizationText("CSS_SEARCH"), "[CHARA SEARCH]", "", ui_char_select_background);
  pCharaSelectSeries = setupComboBoxWidget(CharacterManager::get().GetCharacterPackages() , "character_packages", "[PACKAGE FILTER]");

  connect(char_button_mapper, SIGNAL(mapped(int)), this, SLOT(char_clicked(int)));
  connect(ui_back_to_lobby, SIGNAL(clicked()), this, SLOT(on_back_to_lobby_clicked()));

  connect(ui_chr_select_left, SIGNAL(clicked()), this, SLOT(on_char_select_left_clicked()));
  connect(ui_chr_select_right, SIGNAL(clicked()), this, SLOT(on_char_select_right_clicked()));

  connect(ao_config, SIGNAL(character_ini_changed(QString)), this, SLOT(update_character_icon(QString)));

  connect(ui_spectator, SIGNAL(clicked()), this, SLOT(on_spectator_clicked()));

  connect(pBtnCharSelectRefresh, SIGNAL(clicked()), this, SLOT(OnCharRefreshClicked()));
  connect(pBtnCharSelectRandom, SIGNAL(clicked()), this, SLOT(OnCharRandomClicked()));

  reconstruct_char_select();
}

void Courtroom::reconstruct_char_select()
{

  QPoint f_spacing = ao_app->current_theme->get_widget_settings_spacing("char_buttons", "courtroom", "char_button_spacing");

  const int button_width = 60;
  int x_spacing = f_spacing.x();
  int x_mod_count = 0;

  const int button_height = 60;
  int y_spacing = f_spacing.y();
  int y_mod_count = 0;

  int new_char_columns = ((ui_char_buttons->width() - button_width) / (x_spacing + button_width)) + 1;
  int new_char_rows = ((ui_char_buttons->height() - button_height) / (y_spacing + button_height)) + 1;
  if(new_char_columns == char_columns && new_char_rows == char_rows && CharaSSSpacing == f_spacing)
  {
    return;
  }

  qDeleteAll(ui_char_button_list.begin(), ui_char_button_list.end());

  CharaSSSpacing = f_spacing;
  char_columns = ((ui_char_buttons->width() - button_width) / (x_spacing + button_width)) + 1;
  char_rows = ((ui_char_buttons->height() - button_height) / (y_spacing + button_height)) + 1;

  m_page_max_chr_count = qMax(1, char_columns * char_rows);

  ui_char_button_list.clear();

  for (int n = 0; n < m_page_max_chr_count; ++n)
  {
    int x_pos = (button_width + x_spacing) * x_mod_count;
    int y_pos = (button_height + y_spacing) * y_mod_count;

    AOCharButton *l_button = new AOCharButton(ui_char_buttons, ao_app, x_pos, y_pos);
    ui_char_button_list.append(l_button);

    connect(l_button, SIGNAL(clicked()), char_button_mapper, SLOT(map()));
    char_button_mapper->setMapping(l_button, n);

    // mouse events
    connect(l_button, SIGNAL(mouse_entered(AOCharButton *)), this, SLOT(char_mouse_entered(AOCharButton *)));
    connect(l_button, SIGNAL(mouse_left()), this, SLOT(char_mouse_left()));

    ++x_mod_count;

    if (x_mod_count == char_columns)
    {
      ++y_mod_count;
      x_mod_count = 0;
    }
  }

  reset_char_select();
}

void Courtroom::reset_char_select()
{
  m_current_chr_page = 0;

  set_char_select();
  set_char_select_page();
}

void Courtroom::set_char_select()
{
  pos_size_type f_charselect = ao_app->get_element_dimensions("char_select", COURTROOM_DESIGN_INI);
  if (f_charselect.width < 0 || f_charselect.height < 0)
    qWarning() << "warning: char_select not found or invalid within courtroom_design.ini";

  ui_char_select_background->resize(f_charselect.width, f_charselect.height);
  ui_char_select_background->set_theme_image("charselect_background.png");
}

void Courtroom::set_char_select_page()
{
  ui_char_select_background->show();

  ui_chr_select_left->hide();
  ui_chr_select_right->hide();



  CharacterManager::get().mFilteredChrList = {};


  for (AOCharButton *button : qAsConst(ui_char_button_list))
    button->hide();



  for (char_type charaType : CharacterManager::get().GetCharList(pCharaSelectSeries->currentText()))
  {
    if(charaType.name.toLower().contains(pCharaSelectSearch->text().toLower())) CharacterManager::get().mFilteredChrList.append(charaType);
  }

  const int l_item_count = CharacterManager::get().mFilteredChrList.length();
  const int l_page_count = qFloor(l_item_count / m_page_max_chr_count) + bool(l_item_count % m_page_max_chr_count);
  m_current_chr_page = qBound(0, m_current_chr_page, l_page_count - 1);
  const int l_current_page_emote_count =
      qBound(0, l_item_count - m_current_chr_page * m_page_max_chr_count, m_page_max_chr_count);

  if (m_current_chr_page + 1 < l_page_count)
    ui_chr_select_right->show();

  if (m_current_chr_page > 0)
    ui_chr_select_left->show();

  int yOffset = 0;
  int xOffset = 0;

  UIFilteredCharButton = {};

  // show all buttons for this page
  for (int i = 0; i < l_current_page_emote_count; ++i)
  {

    int l_real_i = i + m_current_chr_page * m_page_max_chr_count;
    AOCharButton *l_button = ui_char_button_list.at(i);
    const QString l_base_chr = CharacterManager::get().mFilteredChrList.at(l_real_i).name;
    l_button->set_character(l_base_chr, ao_config->character_ini(l_base_chr));
    l_button->set_taken((CharacterManager::get().mFilteredChrList.at(l_real_i).taken));
    l_button->show();
    l_button->move(xOffset, yOffset);
    xOffset += 68;
    UIFilteredCharButton.append(l_button);

    if(xOffset + 40 >= ui_char_buttons->width())
    {
      xOffset = 0;
      yOffset += 68;
    }

  }
}

void Courtroom::on_char_select_left_clicked()
{
  --m_current_chr_page;
  set_char_select_page();
}

void Courtroom::on_char_select_right_clicked()
{
  ++m_current_chr_page;
  set_char_select_page();
}

void Courtroom::update_character_icon(QString p_character)
{
  for (AOCharButton *i_button : qAsConst(ui_char_button_list))
  {
    if (i_button->character() != p_character)
      continue;
    i_button->set_character(p_character, ao_config->character_ini(p_character));
    break;
  }
}

void Courtroom::char_clicked(int n_char)
{
  if (get_character() == UIFilteredCharButton.at(n_char)->character())
  {
    enter_courtroom(get_character_id());
    return;
  }

  int n_real_char = n_char + m_current_chr_page * m_page_max_chr_count;

  QString l_pathCharIni = ao_app->get_character_path(CharacterManager::get().mFilteredChrList.at(n_real_char).name, CHARACTER_CHAR_INI);
  QString l_pathCharJson = ao_app->get_character_path(CharacterManager::get().mFilteredChrList.at(n_real_char).name, CHARACTER_CHAR_JSON);

  qDebug() << "char_ini_path" << l_pathCharIni;

  if (!file_exists(l_pathCharIni) && !file_exists(l_pathCharJson) )
  {
    qDebug() << "did not find " << l_pathCharIni;
    call_notice("Could not find " + l_pathCharIni);
    return;
  }

  if(!CharacterManager::get().GetCharacterInServer(n_real_char))
  {
    int filtered_char = n_real_char;
    n_real_char = CharacterManager::get().GetAvaliablePersona();
    ao_config->set_character_ini(CharacterManager::get().GetServerCharaName(n_real_char), CharacterManager::get().GetFilteredCharaName(filtered_char));
  }
  else
  {
    n_real_char = CharacterManager::get().GetFilteredId(n_real_char);
  }

  ao_app->send_server_packet(
      DRPacket("CC", {QString::number(ao_app->get_client_id()), QString::number(n_real_char), "HDID"}));
}

void Courtroom::char_mouse_entered(AOCharButton *p_caller)
{
  if (p_caller == nullptr)
    return;
  ui_char_button_selector->move(p_caller->x() - 1, p_caller->y() - 1);
  ui_char_button_selector->raise();
  ui_char_button_selector->show();
}

void Courtroom::char_mouse_left()
{
  ui_char_button_selector->hide();
}
