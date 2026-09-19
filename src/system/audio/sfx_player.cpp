#include "sfx_player.h"

#include "draudioengine.h"
#include "draudiostream.h"
#include "fs/fs_reading.h"
#include "fs/fs_characters.h"

AOSfxPlayer::AOSfxPlayer(QObject *p_parent)
    : AOObject(p_parent)
    , m_player(DRAudioEngine::get_family(DRAudio::Family::FEffect))
{}

void AOSfxPlayer::play(QString p_filename)
{
  auto l_stream = m_player->play_stream(p_filename);
  if(l_stream)
  {
    m_stream_list.append(l_stream);
  }
}

void AOSfxPlayer::play_effect(QString p_effect)
{
  QString assetPath = ao_app->find_asset_path({"sounds/general/" + p_effect}, FS::Formats::SupportedAudio());
  play(assetPath);
}

void AOSfxPlayer::play_character_effect(QString p_chr, QString p_effect)
{
  QStringList l_file_list;
  for(const QString &i_chr : ao_app->get_char_include_tree(p_chr))
    l_file_list.append(engine::fs::characters::getFilePath(i_chr, QString("sounds/%1").arg(p_effect)));

  const QString l_target_file = ao_app->find_asset_path(l_file_list, FS::Formats::SupportedAudio());
  if(l_target_file.isEmpty())
  {
    play_effect(p_effect);
    return;
  }

  play(l_target_file);
}

void AOSfxPlayer::stop_all()
{
  for(const DRAudioStream::ptr &i_stream : qAsConst(m_stream_list))
  {
    i_stream->stop();
  }
  m_stream_list.clear();
}