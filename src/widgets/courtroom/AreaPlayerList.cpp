#include "AreaPlayerList.h"
#include "drtheme.h"

#include <modules/theme/legacythememanager.h>

AreaPlayerList::AreaPlayerList(QWidget *parent) : QWidget{parent}
{
}

AreaPlayerList::~AreaPlayerList()
{
  deconstruct();
}

void AreaPlayerList::deconstruct()
{
  // Continue to loop through the current PlayerEntries, taking the last
  // so that it can be removed from the list, then deleted.
  while (!m_playerEntries.isEmpty())
    delete m_playerEntries.takeLast();
}

void AreaPlayerList::constructLayout()
{
  deconstruct();
  if(!m_navigationLeft && !m_navigationRight)
    return;

  // TODO: This is using the *old* theme management code, so make sure to update rolechat
  //       so that spacing settings can be retrieved that way.
  QPoint f_spacing = AOApplication::getInstance()->current_theme->get_widget_settings_spacing("player_list", "courtroom", "player_list_spacing");

  engine::system::theme::applyDimensions(this, "player_list", ThemeSceneType::SceneType_Courtroom);

  // TODO: Like above, this resize value should probably be moved to use the NEW theme manager.
  float resize = LegacyThemeManager::get().getResize();

  int player_height = engine::system::theme::getDimensions("player_list_slot", ThemeSceneType::SceneType_Courtroom).height;
  if(player_height == 0) player_height = (int)((float)50 * resize);

  int y_spacing = f_spacing.y();
  int max_pages = ceil((SceneManager::get().mPlayerDataList.count() - 1) / m_pageMax);

  m_playerRows = (( (int)((float)this->height() * resize) - player_height) / (y_spacing + player_height)) + 1;

  m_pageMax = qMax(1, m_playerRows);

  //Manage Arrows (Right)
  m_navigationRight->hide();
  if(m_pageCurrent < max_pages)
  {
    m_navigationRight->show();
  }
  else if(m_pageCurrent > max_pages)
  {
    m_pageCurrent = max_pages;
  }

  //Manage Arrows (Left)
  if(m_pageCurrent <= 0)
  {
    m_pageCurrent = 0;
    m_navigationLeft->hide();
  }
  else m_navigationLeft->show();


  int starting_index = (m_pageCurrent * m_pageMax);

  int last_entry_height = 0;
  for (int n = starting_index; n < SceneManager::get().mPlayerDataList.count(); ++n)
  {
    int y_pos = (last_entry_height + y_spacing) * (n - starting_index);
    DrPlayerListEntry* ui_playername = new DrPlayerListEntry(this, AOApplication::getInstance(), 1, y_pos);
    last_entry_height = ui_playername->height();

    DrPlayer playerData = SceneManager::get().mPlayerDataList.at(n);
    ui_playername->set_character(playerData.m_character, playerData.data.afk);
    ui_playername->set_name(playerData.m_showname);
    ui_playername->setURL(playerData.mURL);
    ui_playername->setID(playerData.m_id);
    ui_playername->setStatus(playerData.mPlayerStatus);
    ui_playername->setOutfit(playerData.m_CharacterOutfit);
    ui_playername->setDiscord(playerData.data.discordSnowflake);
    ui_playername->setContentVersion(playerData.data.contentVersion);

    ui_playername->setMod(playerData.mIPID, playerData.mHDID);

    m_playerEntries.append(ui_playername);
    ui_playername->show();

    if(n == (starting_index + m_pageMax)) break;
  }
}

void AreaPlayerList::startClientTyping(int clientId, bool active)
{
  for(DrPlayerListEntry* player : m_playerEntries)
  {
    if(player->clientId() == clientId)
    {
      player->toggleTyping(active);
      return;
    }
  }
}

void AreaPlayerList::assignNavigationButtons(RPButton *left, RPButton *right)
{
  m_navigationLeft = left;
  m_navigationRight = right;
  connect(m_navigationLeft, &RPButton::clicked, this, &AreaPlayerList::navigationClickedLeft);
  connect(m_navigationRight, &RPButton::clicked, this, &AreaPlayerList::navigationClickedRight);
}

void AreaPlayerList::navigationClickedLeft()
{
  --m_pageCurrent;
  constructLayout();
  emit(navigationComplete());
}

void AreaPlayerList::navigationClickedRight()
{
  ++m_pageCurrent;
  constructLayout();
  emit(navigationComplete());
}
