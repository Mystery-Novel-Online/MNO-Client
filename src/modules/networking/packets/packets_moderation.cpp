#include "packets_moderation.h"
#include "aoapplication.h"
#include "courtroom.h"
#include "debug_functions.h"

#include <modules/managers/localization_manager.h>

void PacketModCalled::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->mod_called(t_Contents.at(0));
}

void PacketKicked::HandleIncoming(QStringList t_Contents)
{
  int f_cid = AOApplication::getInstance()->m_courtroom->get_character_id();
  int remote_cid = t_Contents.at(0).toInt();

  if (f_cid != remote_cid && remote_cid != -1)
    return;

  call_notice(LocalizationManager::get().getLocalizationText("NOTICE_KICKED"));
  AOApplication::getInstance()->leave_server();
  AOApplication::getInstance()->construct_lobby();
  AOApplication::getInstance()->destruct_courtroom();
}

void PacketKickBanned::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->set_ban(t_Contents.at(0).toInt());
}
