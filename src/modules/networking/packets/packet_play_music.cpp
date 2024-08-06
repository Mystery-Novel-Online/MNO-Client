#include "packet_play_music.h"
#include "courtroom.h"
#include <aoapplication.h>

PacketPlayMusic::PacketPlayMusic()
{
}

void PacketPlayMusic::HandleIncoming(QStringList t_Contents)
{
  if (AOApplication::getInstance()->GetCourtroomConstructed() && AOApplication::getInstance()->joined_server())
  {
    AOApplication::getInstance()->m_courtroom->handle_song(t_Contents);
  }
}
