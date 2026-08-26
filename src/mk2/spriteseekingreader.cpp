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

#include "mk2/spriteseekingreader.h"

using namespace mk2;

SpriteSeekingReader::SpriteSeekingReader(QObject *parent)
    : SpriteReader{parent}
    , m_sprite_size(QSize{})
    , m_frame_count{0}
    , m_frame_number{-1}
{}

SpriteSeekingReader::~SpriteSeekingReader()
{}

QSize SpriteSeekingReader::get_sprite_size() const
{
  return m_sprite_size;
}

int SpriteSeekingReader::get_frame_count() const
{
  return m_frame_count;
}

mk2::SpriteFrame SpriteSeekingReader::get_frame(int p_number)
{
  if(!is_valid() || p_number < 0 || p_number >= m_frame_count)
  {
    return SpriteFrame{};
  }

  if(p_number == m_frame_number)
  {
    return m_current_frame;
  }

  { // check whatever we need to rewind the device or not
    const int l_next_frame_number = m_frame_number + 1;
    if(p_number < l_next_frame_number)
    {
      _p_reset_buffer_device();
    }
  }

  // seek frame
  QImage l_image(m_reader.size(), QImage::Format_ARGB32);
  for(; m_frame_number < p_number; ++m_frame_number)
  {
    m_reader.read(&l_image);
    m_current_frame.delay = m_reader.nextImageDelay();
  }
  m_current_frame.image = l_image;

  return m_current_frame;
}

QVector<mk2::SpriteFrame> SpriteSeekingReader::get_frame_list()
{
  QVector<SpriteFrame> l_frame_list;

  if(is_valid())
  {
    l_frame_list.resize(m_reader.imageCount());

    // always seek the frame after the current one
    int l_next_frame_number = m_frame_number + 1;
    for(int i = 0; i < m_frame_count; ++i)
    {
      l_next_frame_number %= m_frame_count;
      l_frame_list.replace(l_next_frame_number, get_frame(l_next_frame_number));
      ++l_next_frame_number;
    }
  }

  return l_frame_list;
}

void SpriteSeekingReader::load()
{
  m_raw_data.clear();
  m_sprite_size = QSize{};
  m_frame_count = 0;

  QIODevice *l_device = get_device();
  const int l_prev_pos = l_device->pos();
  if(!l_device->isOpen() && !l_device->open(QIODevice::ReadOnly))
  {
    set_error(Error::DeviceError);
    return;
  }
  l_device->seek(0);
  m_raw_data = l_device->readAll();
  l_device->seek(l_prev_pos);
  _p_reset_buffer_device();
  if(!m_reader.canRead())
  {
    set_error(Error::InvalidDataError);
    return;
  }
  m_sprite_size = m_reader.size();
  m_frame_count = m_reader.imageCount();
  m_current_frame = SpriteFrame{};
  set_loading_progress(100);
  set_state(State::FullyLoaded);
}

void SpriteSeekingReader::_p_reset_buffer_device()
{
  m_data_buffer.reset(new QBuffer(&m_raw_data));

  if(m_data_buffer->open(QIODevice::ReadOnly))
  {
    m_reader.setDevice(m_data_buffer.data());
    m_frame_number = -1;
  }
  else
  {
    set_error(Error::DeviceError);
  }
}
