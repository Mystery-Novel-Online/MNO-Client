#include "pch.h"

#include "engine/network/metadata/server_metadata.h"
#include "engine/system/localization.h"
#include "engine/fs/fs_reading.h"
#include "engine/fs/fs_characters.h"

#include "aoconfig.h"
#include "debug_functions.h"
#include "drpacket.h"
#include "hardware_functions.h"

#include "theme.h"
#include "drtheme.h"


void Courtroom::construct_char_select()
{
  ui_char_select_background = new AOImageDisplay(this, ao_app);

  ui_char_buttons = new CharacterSelectWidget(ui_char_select_background, ao_app);

  ui_char_button_selector = new AOImageDisplay(ui_char_buttons, ao_app);
  ui_char_button_selector->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_char_button_selector->resize(62, 62);

  ui_back_to_lobby = new RPButton("back_to_lobby", "lobby_return.png", engine::system::localization::getText("CSS_DISCONNECT"), ui_char_select_background);

  ui_chr_select_left = new RPButton("char_select_left", "arrow_left.png", "", ui_char_select_background);
  ui_chr_select_right = new RPButton("char_select_right", "arrow_right.png", "", ui_char_select_background);

  ui_spectator = new RPButton("spectator", "spectator.png", engine::system::localization::getText("CSS_SPECTATE"), ui_char_select_background);

  pBtnCharSelectRandom = new RPButton("char_select_random", "char_random.png", engine::system::localization::getText("CSS_RANDOM"), ui_char_select_background);
  pBtnCharSelectRefresh = new RPButton("char_select_refresh", "char_refresh.png", engine::system::localization::getText("REFRESH"), ui_char_select_background);

  pCharaSelectSearch = new RPLineEdit("character_search", engine::system::localization::getText("CSS_SEARCH"), "[CHARA SEARCH]", ui_char_select_background);
  pCharaSelectSeries = setupComboBoxWidget(CharacterRepository::filterList(), "character_packages", "[PACKAGE FILTER]");

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
  RPRect f_charselect = engine::system::theme::getDimensions("char_select", ThemeSceneType::SceneType_Courtroom);
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

  CharacterRepository::clearFiltered();

  for (AOCharButton *button : qAsConst(ui_char_button_list))
    button->hide();

  int l_item_count = 0;

  for (ActorSelectEntry charaType : CharacterRepository::filteredList(pCharaSelectSeries->currentText()))
  {
    if(QString::fromStdString(charaType.name).toLower().contains(pCharaSelectSearch->text().toLower()))
    {
      l_item_count += 1;
      CharacterRepository::addFiltered(charaType);
    }
  }

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
    QString characterName = CharacterRepository::characterNameFiltered(l_real_i);
    l_button->set_character(characterName, ao_config->character_ini(characterName));
    l_button->set_taken(CharacterRepository::isCharacterAvailable(characterName));
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

void Courtroom::SwitchCharacterByName(const char *characterName)
{
  int serverCharacterId = 0;
  QString characterPathIni = fs::characters::getFilePath(characterName, CHARACTER_CHAR_INI);
  QString characterPathJson = fs::characters::getFilePath(characterName, CHARACTER_CHAR_JSON);

  if (user::GetCharacterName() == characterName)
  {
    enter_courtroom(user::GetCharacterId());
    return;
  }

  if (!FS::Checks::FileExists(characterPathIni) && !FS::Checks::FileExists(characterPathJson))
  {
    qDebug() << "did not find " << characterPathIni;
    call_notice("Could not find " + characterPathIni);
    return;
  }

  if(!CharacterRepository::characterExists(characterName))
  {
    serverCharacterId = CharacterRepository::findAvailablePersona();
    if(serverCharacterId == -1) return;
    ao_config->set_character_ini(CharacterRepository::characterNameServer(serverCharacterId), characterName);
  }
  else
  {
    serverCharacterId = CharacterRepository::networkedIdFromName(characterName);
  }

  ao_app->send_server_packet(
      DRPacket("CC", {QString::number(user::getOutgoingClientId()), QString::number(serverCharacterId), "HDID"}));

}

void Courtroom::char_clicked(int n_char)
{
  using namespace engine::network::metadata;
  if (user::GetCharacterName() == UIFilteredCharButton.at(n_char)->character())
  {
    enter_courtroom(user::GetCharacterId());
    return;
  }

  int n_real_char = n_char + m_current_chr_page * m_page_max_chr_count;

  QString l_pathCharIni = fs::characters::getFilePath(CharacterRepository::characterNameFiltered(n_real_char), CHARACTER_CHAR_INI);
  QString l_pathCharJson = fs::characters::getFilePath(CharacterRepository::characterNameFiltered(n_real_char), CHARACTER_CHAR_JSON);

  qDebug() << "char_ini_path" << l_pathCharIni;

  if (!FS::Checks::FileExists(l_pathCharIni) && !FS::Checks::FileExists(l_pathCharJson) )
  {
    qDebug() << "did not find " << l_pathCharIni;
    call_notice("Could not find " + l_pathCharIni);
    return;
  }

  if(!CharacterRepository::filteredCharacterExists(n_real_char))
  {
    int filtered_char = n_real_char;
    n_real_char = CharacterRepository::findAvailablePersona();
    if(n_real_char == -1) return;
    ao_config->set_character_ini(CharacterRepository::characterNameServer(n_real_char), CharacterRepository::characterNameFiltered(filtered_char));
  }
  else
  {
    n_real_char = CharacterRepository::networkedIdFromFiltered(n_real_char);
  }

  ao_app->send_server_packet(
      DRPacket("CC", {QString::number(user::getOutgoingClientId()), QString::number(n_real_char), "HDID"}));
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
