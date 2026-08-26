/**************************************************************************
**
** mk2
** Copyright (C) 2022 Tricky Leifa
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Affero General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
**************************************************************************/

#include "mk2/spritecachingreader.h"

#include <QElapsedTimer>
#include <QImageReader>
#include <QMutexLocker>
#include <QSemaphoreReleaser>
#include <QtConcurrent/QtConcurrentRun>

#include <QBuffer>

using namespace mk2;

SpriteCachingReader::SpriteCachingReader(QObject *parent)
    : SpriteReader{parent}
    , m_sprite_size{}
    , m_frame_count{0}
    , m_exit_task{false}
{}

SpriteCachingReader::~SpriteCachingReader()
{
  _p_stop_preload();
}

QSize SpriteCachingReader::get_sprite_size() const
{
  return m_sprite_size;
}

int SpriteCachingReader::get_frame_count() const
{
  return m_frame_count;
}

SpriteFrame SpriteCachingReader::get_frame(int p_number)
{
  if(!is_valid())
  {
    return SpriteFrame{};
  }
  p_number = qBound(0, p_number, qMax(m_frame_count - 1, 0));
  m_available_frames.acquire(p_number + 1);
  QSemaphoreReleaser l_releaser(m_available_frames, p_number + 1);
  QMutexLocker l_locker(&m_lock);
  return m_frame_list.at(p_number);
}

QVector<SpriteFrame> SpriteCachingReader::get_frame_list()
{
  m_available_frames.acquire(get_frame_count());
  QSemaphoreReleaser l_releaser(m_available_frames, get_frame_count());
  QMutexLocker l_locker(&m_lock);
  return m_frame_list;
}

void SpriteCachingReader::load()
{
  _p_stop_preload();
  m_available_frames.acquire(m_available_frames.available());
  m_sprite_size = QSize{};
  m_frame_count = 0;
  m_frame_list.clear();

  QByteArray l_raw_data;
  QIODevice *l_device = get_device();
  const int l_prev_pos = l_device->pos();
  if(!l_device->isOpen() && !l_device->open(QIODevice::ReadOnly))
  {
    set_error(Error::DeviceError);
    return;
  }
  l_device->seek(0);
  l_raw_data = l_device->readAll();
  l_device->seek(l_prev_pos);

  {
    QBuffer l_buffer(&l_raw_data);
    QImageReader l_reader(&l_buffer);
    if(!l_reader.canRead())
    {
      set_error(Error::InvalidDataError);
      return;
    }
    m_sprite_size = l_reader.size();
    m_frame_count = l_reader.imageCount();
  }

  m_exit_task = false;
  m_task = QtConcurrent::run(this, &SpriteCachingReader::_p_preload, l_raw_data);
}

void SpriteCachingReader::_p_stop_preload()
{
  m_exit_task = true;
  m_task.waitForFinished();
}

void SpriteCachingReader::_p_preload(QByteArray p_raw_data)
{
  set_state(State::NotLoaded);
  set_loading_progress(0);

  QBuffer l_buffer(&p_raw_data);
  QImageReader l_reader(&l_buffer);
  const QSize l_size = l_reader.size();
  const int l_frame_count = l_reader.imageCount();
  if(l_frame_count > 0)
  {
    QElapsedTimer l_elapsed_timer;
    l_elapsed_timer.start();

    // create a buffer for images
    QVector<QImage> l_image_buffer_list;
    for(int i = 0; i < l_frame_count; ++i)
    {
      l_image_buffer_list.append(QImage(l_size, QImage::Format_ARGB32));
    }

    int l_frame_number = 0;
    int l_percent_progress = 0;
    while(!m_exit_task && l_frame_number < l_frame_count && l_reader.canRead())
    {
      SpriteFrame l_frame;
      QImage l_image_buffer = l_image_buffer_list.takeFirst();
      l_reader.read(&l_image_buffer);
      l_frame.image = l_image_buffer;
      l_frame.delay = l_reader.nextImageDelay();
      {
        QMutexLocker locker(&m_lock);
        m_frame_list.append(std::move(l_frame));
        l_frame_number = m_frame_list.length();
        m_available_frames.release();
      }

      l_percent_progress = ((double)l_frame_number / (l_frame_count + 1)) * 100;
      set_loading_progress(l_percent_progress);
    }

    if(!m_exit_task)
    {
      set_loading_progress(100);
      set_state(State::FullyLoaded);
    }
  }
  else
  {
    set_error(Error::InvalidDataError);
  }
}
