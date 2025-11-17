#include "bgm_menu.h"
#include <modules/theme/thememanager.h>
#include "engine/network/metadata/tracklist_metadata.h"
#include "engine/interface/courtroom_layout.h"
#include "engine/system/audio.h"

BGMMenu::BGMMenu(QWidget *parent) : QMenu(parent)
{

  QMenu *playMenu = new QMenu(tr("Play"), this);
  addMenu(playMenu);

  m_PlaySmooth = playMenu->addAction((tr("Smooth Play")));
  m_PlayInstant = playMenu->addAction((tr("Instant Play")));
  m_PlaySync = playMenu->addAction((tr("Sync Playback")));
  m_PlaySolo = playMenu->addAction((tr("Solo Playback")));



  p_InsertAction = addAction(tr("Insert into OOC"));
  p_PinAction = addAction(tr("Pin"));

  connect(m_PlaySmooth, &QAction::triggered, this, &BGMMenu::OnSmoothPlayAction);
  connect(m_PlayInstant, &QAction::triggered, this, &BGMMenu::OnInstantPlayAction);
  connect(m_PlaySync, &QAction::triggered, this, &BGMMenu::OnSyncPlayAction);
  connect(m_PlaySolo, &QAction::triggered, this, &BGMMenu::OnSoloPlayAction);

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

void BGMMenu::OnSmoothPlayAction()
{
  if(m_TargetTrack.isEmpty()) return;
  AOApplication::getInstance()->get_courtroom()->send_mc_packet(m_TargetTrack, BGMPlayback_Standard);
  courtroom::ic::focusMessageBox();
}

void BGMMenu::OnInstantPlayAction()
{
  if(m_TargetTrack.isEmpty()) return;
  AOApplication::getInstance()->get_courtroom()->send_mc_packet(m_TargetTrack, BGMPlayback_NoFade);
  courtroom::ic::focusMessageBox();
}

void BGMMenu::OnSyncPlayAction()
{
  if(m_TargetTrack.isEmpty()) return;
  AOApplication::getInstance()->get_courtroom()->send_mc_packet(m_TargetTrack, BGMPlayback_Continue);
  courtroom::ic::focusMessageBox();
}

void BGMMenu::OnSoloPlayAction()
{
  if(m_TargetTrack.isEmpty()) return;
  audio::bgm::Play(m_TargetTrack.toStdString());
  courtroom::ic::focusMessageBox();
}

void BGMMenu::OnPinTriggered()
{
  if(m_TargetTrack.isEmpty()) return;
  TracklistMetadata::PinTrack(m_TargetTrack);
}

