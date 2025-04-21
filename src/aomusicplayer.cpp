#include "aomusicplayer.h"

#include "aoapplication.h"

#include "draudiostream.h"
#include "draudiotrackmetadata.h"
#include <QDebug>

AOMusicPlayer::AOMusicPlayer(AOApplication *p_ao_app, QObject *p_parent)
    : AOObject(p_ao_app, p_parent)
{
  m_family = DRAudioEngine::get_family(DRAudio::Family::FMusic);
  m_family->set_capacity(1); // a single song is needed
}

void AOMusicPlayer::play(QString p_song)
{
  stop();

  m_filename = p_song;

  if(mLastSong != nullptr)
  {
    mLastSong->stop();
  }
  mLastSong = mCurrentSong;

  if(mLastSong != nullptr)
  {
    mLastSong->fadeOut(3000);
  }

  mCurrentSong = m_family->create_stream(ao_app->get_music_path(p_song));
  if (mCurrentSong)
  {
    DRAudiotrackMetadata l_audiotrack(p_song);
    if (!l_audiotrack.play_once())
    {
      mCurrentSong->set_repeatable(true);
      mCurrentSong->set_loop(l_audiotrack.loop_start(), l_audiotrack.loop_end());
    }

    mCurrentSong->fadeIn(3000);
    mCurrentSong->play();

    if (mCurrentSong->is_playing())
    {
      qDebug() << "playing" << mCurrentSong->get_file_name();
    }
  }
}

void AOMusicPlayer::stop()
{
  //if(mLastSong == nullptr)
  //{
  //  for (auto &song : m_family->get_stream_list())
  //    song->stop();
  //}
}
