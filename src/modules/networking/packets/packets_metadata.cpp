#include "packets_metadata.h"
#include "courtroom.h"
#include "lobby.h"

#include <modules/managers/character_manager.h>

#include <aoapplication.h>

void PacketMusicList::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->set_music_list(ScenarioManager::get().ParseMusicList(t_Contents));

  if (!AOApplication::getInstance()->GetLoadedAreaList() && AOApplication::getInstance()->GetLobbyConstructed())
  {
    AOApplication::getInstance()->m_lobby->set_loading_text("Loading music...");
    AOApplication::getInstance()->send_server_packet(DRPacket("RD"));
  }
  AOApplication::getInstance()->SetLoadedMusicList(true);
}

void PacketAreaList::HandleIncoming(QStringList t_Contents)
{
  AOApplication::getInstance()->m_courtroom->set_area_list(t_Contents);

  if (!AOApplication::getInstance()->GetLoadedAreaList() && AOApplication::getInstance()->GetLobbyConstructed())
  {
    AOApplication::getInstance()->m_lobby->set_loading_text("Loading areas...");
  }
  AOApplication::getInstance()->SetLoadedAreaList(true);
}

void PacketCharaList::HandleIncoming(QStringList t_Contents)
{
  Lobby *l_ClientLobby = AOApplication::getInstance()->m_lobby;

  QVector<char_type> l_chr_list;
  for (const QString &i_chr_name : qAsConst(t_Contents))
  {
    char_type l_chr;
    l_chr.name = i_chr_name;
    l_chr_list.append(std::move(l_chr));
  }
  CharacterManager::get().SetCharList(l_chr_list);

  int l_CharacterCount = AOApplication::getInstance()->GetCharacterCount();
  int l_MusicCount = AOApplication::getInstance()->GetMusicCount();

  AOApplication::getInstance()->SetLoadedCharacterCount(l_CharacterCount);

  if (AOApplication::getInstance()->GetLobbyConstructed())
  {
    l_ClientLobby->set_loading_text("Loading chars:\n" + QString::number(AOApplication::getInstance()->GetLoadedCharactersCount()) + "/" + QString::number(l_CharacterCount));
    int total_loading_size = l_CharacterCount + 0 + l_MusicCount;
    int loading_value = (AOApplication::getInstance()->GetLoadedCharactersCount() / static_cast<double>(total_loading_size)) * 100;
    l_ClientLobby->set_loading_value(loading_value);

    AOApplication::getInstance()->send_server_packet(DRPacket("RM"));
  }
}

void PacketCharaCheck::HandleIncoming(QStringList t_Contents)
{
  QVector<char_type> l_chr_list = CharacterManager::get().GetServerCharList();
  if (t_Contents.length() != l_chr_list.length())
  {
    qWarning() << "Server sent a character list of length " << t_Contents.length() << "which is different from the expected length " << l_chr_list.length() << "so ignoring it.";
    return;
  }

  for (int i = 0; i < l_chr_list.length(); ++i)
  {
    l_chr_list[i].taken = t_Contents.at(i) == "-1";
    CharacterManager::get().SetCharaTaken(i, t_Contents.at(i) == "-1");
  }

  CharacterManager::get().SetCharList(l_chr_list);
}
