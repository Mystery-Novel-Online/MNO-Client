#include "shout_player.h"

#include "draudioengine.h"
#include "dro/fs/fs_reading.h"
#include "dro/fs/fs_characters.h"

AOShoutPlayer::AOShoutPlayer(QObject *p_parent)
    : AOObject(p_parent)
{}

void AOShoutPlayer::play(QString p_chr, QString p_shout)
{
  QStringList l_file_list;
  for (const QString &i_chr : ao_app->get_char_include_tree(p_chr))
    l_file_list.append(fs::characters::getFilePath(i_chr, p_shout));

  QString l_file = ao_app->find_asset_path(l_file_list, FS::Formats::SupportedAudio());
  if (l_file.isEmpty())
    l_file = ao_app->find_theme_asset_path(p_shout, FS::Formats::SupportedAudio());

  if (l_file.isEmpty())
    return;
  DRAudioEngine::get_family(DRAudio::Family::FEffect)->play_stream(l_file);
}
