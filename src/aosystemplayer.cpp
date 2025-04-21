#include "aosystemplayer.h"

#include "aoapplication.h"
#include "draudioengine.h"
#include "dro/fs/fs_reading.h"
#include "dro/fs/fs_reading.h"

AOSystemPlayer::AOSystemPlayer(AOApplication *p_ao_app, QObject *p_parent)
    : AOObject(p_ao_app, p_parent)
{}

void AOSystemPlayer::play(QString p_name)
{
  const QString file = ao_app->find_asset_path({ao_app->get_sfx_noext_path(p_name)}, FS::Formats::SupportedAudio());
  DRAudioEngine::get_family(DRAudio::Family::FSystem)->play_stream(file);
}
