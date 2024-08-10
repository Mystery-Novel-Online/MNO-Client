#include "packets_metadata.h"

#include <modules/managers/character_manager.h>

void PacketMusicList::HandleIncoming(QStringList t_Contents)
{

}

void PacketAreaList::HandleIncoming(QStringList t_Contents)
{

}

void PacketCharaList::HandleIncoming(QStringList t_Contents)
{

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
