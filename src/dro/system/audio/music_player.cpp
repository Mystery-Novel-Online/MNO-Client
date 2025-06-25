#include "music_player.h"

#include "aoapplication.h"

#include "draudiostream.h"
#include "draudiotrackmetadata.h"
#include <QDebug>
#include <QRegularExpression>

AOMusicPlayer::AOMusicPlayer(QObject *p_parent)
    : AOObject(p_parent)
{
  m_family = DRAudioEngine::get_family(DRAudio::Family::FMusic);
  m_family->set_capacity(1); // a single song is needed
}

void AOMusicPlayer::play(QString p_song, BGMPlayback playbackType)
{
  m_filename = p_song;


  QRegularExpression urlRegex("(http|https|ftp)://[^\\s/$.?#].[^\\s]*");
  QRegularExpressionMatch match = urlRegex.match(m_filename);


  QSharedPointer<DRAudioStream> newSong;
  if(match.hasMatch())
  {
    newSong = m_family->create_url_stream(p_song);
  }
  else
  {
    newSong = m_family->create_stream(ao_app->get_music_path(p_song));
  }


  if(!newSong)
  {
    switch(playbackType)
    {
      case BGMPlayback_NoFade:
        stop();
        break;

      default:
        if (mCurrentSong)
          mCurrentSong->fadeOut(3000);
        break;
    }
    return;
  }

  DRAudiotrackMetadata metadata(p_song);
  if (!metadata.play_once())
  {
    newSong->set_repeatable(true);
    newSong->set_loop(metadata.loop_start(), metadata.loop_end());
  }

  switch(playbackType)
  {
    case BGMPlayback_NoFade:
      stop();
      newSong->play();
      break;

    case BGMPlayback_Continue:
      if (mCurrentSong)
        mCurrentSong->fadeOut(300);
      newSong->fadeIn(250);
      newSong->playSynced(mCurrentSong.data());
      break;

    case BGMPlayback_Standard:
    default:
      if (mCurrentSong)
        mCurrentSong->fadeOut(3000);
      newSong->fadeIn(3000);
      newSong->play();
      break;
  }

    if (newSong->is_playing())
      qDebug() << "playing" << newSong->get_file_name();

    newSong->set_speed(0.0f);
    newSong->toggle_reverb(false);

    mLastSong = mCurrentSong;
    mCurrentSong = newSong;
}

void AOMusicPlayer::setSpeed(float speed)
{
  if(mCurrentSong == nullptr) return;
  if (mCurrentSong->is_playing())
  {
    mCurrentSong->set_speed(speed);
  }
}

void AOMusicPlayer::toggleReverb(bool reverb)
{
  if(mCurrentSong == nullptr) return;
  if (mCurrentSong->is_playing())
  {
    mCurrentSong->toggle_reverb(true);
  }
}

void AOMusicPlayer::setPitch(float pitch)
{
  if(mCurrentSong == nullptr) return;
  if (mCurrentSong->is_playing())
  {
    mCurrentSong->set_pitch(pitch);
  }
}

void AOMusicPlayer::stop()
{
  if(mLastSong)
    mLastSong->stop();
  if(mCurrentSong)
    mCurrentSong->stop();
}
