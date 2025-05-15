#include "sfx_player.h"

#include "aoapplication.h"
#include "draudioengine.h"
#include "draudiostream.h"
#include "dro/fs/fs_reading.h"
#include "dro/fs/fs_reading.h"

#include <QDebug>
#include <cstddef>

AOSfxPlayer::AOSfxPlayer(QObject *p_parent)
    : AOObject(p_parent)
    , m_player(DRAudioEngine::get_family(DRAudio::Family::FEffect))
{}

void AOSfxPlayer::play(QString p_filename)
{
  auto l_stream = m_player->play_stream(p_filename);
  if (l_stream)
  {
    qWarning() << "Playing effect" << p_filename;
    m_stream_list.append(l_stream);
  }
}

void AOSfxPlayer::play_effect(QString p_effect)
{
  play(ao_app->find_asset_path({ao_app->get_sfx_noext_path(p_effect)}, FS::Formats::SupportedAudio()));
}

void AOSfxPlayer::play_character_effect(QString p_chr, QString p_effect)
{
  QStringList l_file_list;
  for (const QString &i_chr : ao_app->get_char_include_tree(p_chr))
    l_file_list.append(ao_app->get_character_path(i_chr, QString("sounds/%1").arg(p_effect)));

  const QString l_target_file = ao_app->find_asset_path(l_file_list, FS::Formats::SupportedAudio());
  if (l_target_file.isEmpty())
  {
    play_effect(p_effect);
    return;
  }

  play(l_target_file);
}

void AOSfxPlayer::stop_all()
{
  for (const DRAudioStream::ptr &i_stream : qAsConst(m_stream_list))
  {
    i_stream->stop();
  }
  m_stream_list.clear();
}

void AOSfxPlayer::play_ambient(QString p_filename)
{
  if (m_current_ambient)
  {
    if (m_current_ambient->get_file_name() == p_filename)
    {
      return;
    }

    m_current_ambient->fadeOut(DEFAULT_FADE_DURATION);
  }

  DRAudioStream::ptr l_ambient;
  if (!m_ambient_map.contains(p_filename))
  {
    l_ambient = m_player->create_stream(p_filename);

    if (l_ambient)
    {
      qInfo() << "Playing ambient" << p_filename;
      m_ambient_map.insert(p_filename, l_ambient);

      connect(l_ambient.data(), SIGNAL(faded(DRAudioStream::Fade)), this, SLOT(handle_ambient_fade(DRAudioStream::Fade)));
      connect(l_ambient.data(), SIGNAL(finished()), this, SLOT(remove_ambient()));

      l_ambient->set_repeatable(true);
    }
    else
    {
      return;
    }
  }
  else
  {
    qInfo() << "Restoring ambient" << p_filename;
    l_ambient = m_ambient_map[p_filename];
  }
  m_current_ambient = l_ambient;

  if (m_current_ambient.isNull())
  {
    return;
  }

  m_current_ambient->fadeIn(DEFAULT_FADE_DURATION);

  if (!m_current_ambient->is_playing())
  {
    m_current_ambient->play();
  }
}

DRAudioStream::ptr AOSfxPlayer::get_stream_by_qobject(QObject *p_object)
{
  auto *l_stream_ptr = dynamic_cast<DRAudioStream *>(p_object);
  if (!l_stream_ptr)
  {
    qCritical() << "error: object was not an audio stream" << p_object;
    return nullptr;
  }

  for (auto it = m_ambient_map.cbegin(); it != m_ambient_map.cend(); ++it)
  {
    const auto &i_stream = it.value();
    if (l_stream_ptr == i_stream)
    {
      return i_stream;
    }
  }

  return nullptr;
}

void AOSfxPlayer::remove_ambient()
{
  auto l_stream = get_stream_by_qobject(sender());
  if (l_stream.isNull())
  {
    return;
  }

  qDebug() << "Removing ambient" << l_stream->get_file_name();
  m_ambient_map.remove(l_stream->get_file_name());
  if (m_current_ambient == l_stream)
  {
    m_current_ambient.reset();
  }
}

void AOSfxPlayer::handle_ambient_fade(DRAudioStream::Fade p_fade)
{
  const auto l_stream = get_stream_by_qobject(sender());
  if (l_stream.isNull())
  {
    return;
  }

  if (p_fade == DRAudioStream::FadeOut)
  {
    l_stream->stop();
  }
}
