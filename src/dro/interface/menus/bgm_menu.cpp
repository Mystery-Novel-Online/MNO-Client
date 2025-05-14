#include "bgm_menu.h"

#include <QListWidget>

#include <modules/theme/thememanager.h>
#include "courtroom.h"
#include "dro/network/metadata/tracklist_metadata.h"

BGMMenu::BGMMenu(QWidget *parent) : QMenu(parent)
{
  p_PlayAction = addAction(tr("Play"));
  p_InsertAction = addAction(tr("Insert into OOC"));
  p_PinAction = addAction(tr("Pin"));

  connect(p_PlayAction, &QAction::triggered, this, &BGMMenu::OnPlayTriggered);
  connect(p_InsertAction, &QAction::triggered, this, &BGMMenu::OnInsertTriggered);
  connect(p_PinAction, &QAction::triggered, this, &BGMMenu::OnPinTriggered);
}

void BGMMenu::OnMenuRequested(QPoint p_point)
{
  QListWidget *musicList = ThemeManager::get().GetWidgetType<QListWidget>("music_list");
  if(musicList == nullptr) return;

  QListWidgetItem *l_item = musicList->currentItem();

  if (!l_item) { m_TargetTrack = "";  return; }

  m_TargetTrack = l_item->data(Qt::UserRole).toString();
  const QPoint l_global_point = musicList->viewport()->mapToGlobal(p_point);
  popup(l_global_point);
}

void BGMMenu::OnInsertTriggered()
{
  if(m_TargetTrack.isEmpty()) return;
  QLineEdit *oocChat = ThemeManager::get().GetWidgetType<QLineEdit>("ooc_chat_message");
  oocChat->insert(m_TargetTrack);
  oocChat->setFocus();
}

void BGMMenu::OnPlayTriggered()
{
  if(m_TargetTrack.isEmpty()) return;
  AOApplication::getInstance()->get_courtroom()->send_mc_packet(m_TargetTrack);
  //ui_ic_chat_message_field->setFocus();
}

void BGMMenu::OnPinTriggered()
{
  if(m_TargetTrack.isEmpty()) return;
  TracklistMetadata::PinTrack(m_TargetTrack);
}

