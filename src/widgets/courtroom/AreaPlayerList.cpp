#include "AreaPlayerList.h"
#include "drtheme.h"
#include <modules/theme/legacythememanager.h>

#include "AreaPlayerEntry.h"

AreaPlayerList::AreaPlayerList(QWidget *a_parent)
  : QWidget(a_parent) {
}

AreaPlayerList::~AreaPlayerList() {
  deconstruct();
}

void AreaPlayerList::deconstruct(bool a_destroyAll) {
  QSet<int> currentPlayerIds;

  if(!a_destroyAll) {
    for(const DrPlayer &player : SceneManager::get().m_areaPlayers) {
      currentPlayerIds.insert(player.data.id);
    }
  }

  for(auto it = u_playerEntries.begin(); it != u_playerEntries.end();)
  {
    if(!currentPlayerIds.contains(it.key())) {
      delete it.value();
      it = u_playerEntries.erase(it);
    }
    else {
      ++it;
    }
  }
}

void AreaPlayerList::constructLayout() {
  //TODO: We shouldn't delete every player entry, see if it's more performant to add checks if
  //      a user present in the latest network update before deleting.
  deconstruct();

  if(!u_navigationLeft && !u_navigationRight) {
    return;
  }

  engine::system::theme::applyDimensions(this, "player_list", ThemeSceneType::SceneType_Courtroom);

  m_playerSpacing = AOApplication::getInstance()->current_theme->get_widget_settings_spacing("player_list", "courtroom", "player_list_spacing").y();

  m_playerRows = calculateEntryCount();
  updatePageNavigation();
  populatePlayers();
}

void AreaPlayerList::startClientTyping(int a_targetClient, bool a_activeState) {
  for(AreaPlayerEntry* player : u_playerEntries) {
    if(player->clientId() == a_targetClient) {
      player->toggleTyping(a_activeState);
      return;
    }
  }
}

void AreaPlayerList::assignNavigationButtons(RPButton *a_leftButton, RPButton *a_rightButton) {
  u_navigationLeft = a_leftButton;
  u_navigationRight = a_rightButton;
  connect(u_navigationLeft, &RPButton::clicked, this, &AreaPlayerList::navigationClickedLeft);
  connect(u_navigationRight, &RPButton::clicked, this, &AreaPlayerList::navigationClickedRight);
}

void AreaPlayerList::updatePageNavigation() {
  int max_pages = ceil((SceneManager::get().m_areaPlayers.count() - 1) / m_pageMax);
  m_pageMax = qMax(1, m_playerRows);

  u_navigationRight->hide();
  if(m_pageCurrent < max_pages) {
    u_navigationRight->show();
  }
  else if(m_pageCurrent > max_pages) {
    m_pageCurrent = max_pages;
  }

  if(m_pageCurrent <= 0) {
    m_pageCurrent = 0;
    u_navigationLeft->hide();
  }
  else {
    u_navigationLeft->show();
  }
}

void AreaPlayerList::populatePlayers() {
  int starting_index = (m_pageCurrent * m_pageMax);

  int last_entry_height = 0;

  for(auto *entry : u_playerEntries) {
    entry->hide();
  }

  for(int n = starting_index; n < SceneManager::get().m_areaPlayers.count(); ++n) {
    int y_pos = (last_entry_height + m_playerSpacing) * (n - starting_index);
    DrPlayer playerData = SceneManager::get().m_areaPlayers.at(n);

    auto it = u_playerEntries.find(playerData.data.id);
    if(it != u_playerEntries.end()) {
      AreaPlayerEntry *entry = it.value();
      entry->updateData(playerData, y_pos);
      entry->show();
      last_entry_height = entry->height();
    }
    else {
      AreaPlayerEntry* ui_playername = new AreaPlayerEntry(this, AOApplication::getInstance(), 1, y_pos, playerData);
      last_entry_height = ui_playername->height();
      u_playerEntries.insert(playerData.data.id, ui_playername);
      ui_playername->show();
    }

    if(n == (starting_index + m_pageMax)) {
      break;
    }
  }
}

int AreaPlayerList::calculateEntryCount() {
  float resize = LegacyThemeManager::get().getResize();

  int player_height = engine::system::theme::getDimensions("player_list_slot", ThemeSceneType::SceneType_Courtroom).height;

  // We default to a size of 50 here if the theme creator has not defined a custom size.
  if(player_height == 0) {
    player_height = (int)((float)50 * resize);
  }
  // Calculate how many entries will be able to fit vertically within this widgets dimensions.
  return (( (int)((float)this->height() * resize) - player_height) / (m_playerSpacing + player_height)) + 1;
}


void AreaPlayerList::navigationClickedLeft() {
  --m_pageCurrent;
  constructLayout();
  emit(navigationComplete());
}

void AreaPlayerList::navigationClickedRight() {
  ++m_pageCurrent;
  constructLayout();
  emit(navigationComplete());
}
