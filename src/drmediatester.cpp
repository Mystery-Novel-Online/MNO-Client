#include "drmediatester.h"
#include "pch.h"
#include "debug_functions.h"

DRMediaTester::DRMediaTester(QObject *parent)
    : QObject(parent)
{
  m_player.setMuted(true);

  connect(&m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(p_check_status(QMediaPlayer::MediaStatus)));

  m_player.setMedia(QUrl("qrc:/data/sample.avi"));
}

DRMediaTester::~DRMediaTester()
{}

void DRMediaTester::p_check_status(QMediaPlayer::MediaStatus p_status)
{
  switch (p_status)
  {
  case QMediaPlayer::InvalidMedia:
    call_warning("Your operating system appears to not currently support video playback. The video "
                 "playback will not work properly.<br /><br />In order for the feature to function properly, you "
                 "will need to install additional codecs.<br /><br />For more information visit "
                 "<a href=\"https://www.danganronpaonline.com\">https://www.danganronpaonline.com</a>");
    emit done();
    break;

  case QMediaPlayer::LoadedMedia:
    emit done();
    break;

  default:
    break;
  }
}
