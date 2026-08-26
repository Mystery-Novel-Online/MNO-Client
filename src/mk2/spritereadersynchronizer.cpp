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

#include "mk2/spritereadersynchronizer.h"

using namespace mk2;

SpriteReaderSynchronizer::SpriteReaderSynchronizer(QObject *parent)
    : QObject{parent}
    , m_waiting{false}
    , m_finished{false}
    , m_threshold{50}
{}

SpriteReaderSynchronizer::~SpriteReaderSynchronizer()
{}

int SpriteReaderSynchronizer::get_threshold() const
{
  return m_threshold;
}

QVector<SpriteReader::ptr> SpriteReaderSynchronizer::get_reader_list() const
{
  return m_reader_list;
}

bool SpriteReaderSynchronizer::is_waiting() const
{
  return m_waiting;
}

bool SpriteReaderSynchronizer::is_finished() const
{
  return m_finished;
}

void SpriteReaderSynchronizer::set_threshold(int p_threshold)
{
  p_threshold = qBound(0, p_threshold, 100);
  if(m_threshold == p_threshold)
  {
    return;
  }
  const int l_prev_threshold = m_threshold;
  m_threshold = p_threshold;
  if(m_threshold < l_prev_threshold)
  {
    _p_check_progress();
  }
}

void SpriteReaderSynchronizer::add(mk2::SpriteReader::ptr p_reader)
{
  if(m_reader_list.contains(p_reader))
  {
    return;
  }
  m_finished = false;
  m_reader_list.append(p_reader);
  connect(p_reader.data(), SIGNAL(loading_progress_changed(int)), this, SLOT(_p_check_progress()));
  _p_check_progress();
}

void SpriteReaderSynchronizer::clear()
{
  m_waiting = false;
  m_finished = false;
  for(const mk2::SpriteReader::ptr &i_reader : qAsConst(m_reader_list))
  {
    i_reader->disconnect(this);
  }
  m_reader_list.clear();
}

void SpriteReaderSynchronizer::start()
{
  m_waiting = true;
  if(m_finished)
  {
    emit finished();
  }
  else
  {
    _p_check_progress();
  }
}

void SpriteReaderSynchronizer::_p_check_progress()
{
  if(m_finished || m_reader_list.isEmpty())
  {
    return;
  }

  for(const mk2::SpriteReader::ptr &i_reader : qAsConst(m_reader_list))
  {
    // if the reader is invalid, it's the same as being fully loaded
    if(i_reader->is_valid() && i_reader->get_loading_progress() < m_threshold)
    {
      return;
    }
  }

  m_finished = true;
  if(m_waiting)
  {
    emit finished();
  }
}
