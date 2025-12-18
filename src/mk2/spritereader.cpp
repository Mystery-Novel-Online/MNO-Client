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

#include "mk2/spritereader.h"

#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QImageReader>

using namespace mk2;

SpriteFrame::SpriteFrame()
    : delay{0}
{}

SpriteFrame::~SpriteFrame()
{}

void SpriteReader::registerMetatypes()
{
  static bool s_registerRequired = true;
  if (s_registerRequired)
  {
    s_registerRequired = false;
    qRegisterMetaType<mk2::SpriteReader::State>();
    qRegisterMetaType<mk2::SpriteReader::Error>();
  }
}

SpriteReader::SpriteReader(QObject *parent)
    : QObject{parent}
    , m_device{new QFile}
    , m_own_device{true}
    , m_state{State::NotLoaded}
    , m_last_error{Error::NoError}
    , m_loading_progress{0}
{
  registerMetatypes();
}

SpriteReader::~SpriteReader()
{
  _p_delete_device();
}

QString SpriteReader::get_file_name() const
{
  QFile *l_file = dynamic_cast<QFile *>(m_device);
  return l_file ? l_file->fileName() : QString{};
}

QIODevice *SpriteReader::get_device() const
{
  return m_device;
}

bool SpriteReader::is_valid() const
{
  return get_frame_count() > 0;
}

QSize SpriteReader::get_sprite_size() const
{
  return QSize{};
}

int SpriteReader::get_frame_count() const
{
  return 0;
}

SpriteFrame SpriteReader::get_frame(int p_number)
{
  Q_UNUSED(p_number);
  return SpriteFrame{};
}

QVector<SpriteFrame> SpriteReader::get_frame_list()
{
  return QVector<SpriteFrame>{};
}

SpriteReader::State SpriteReader::get_state() const
{
  return m_state;
}

bool SpriteReader::is_loaded() const
{
  return m_state == State::FullyLoaded;
}

int SpriteReader::get_loading_progress() const
{
  return m_loading_progress;
}

SpriteReader::Error SpriteReader::get_last_error() const
{
  return m_last_error;
}

void SpriteReader::set_file_name(QString p_file_name)
{
  set_device(new QFile(p_file_name));
  m_own_device = true;

  QFileInfo fi(p_file_name);
  QString maskFileName = fi.path() + "/" + fi.completeBaseName() + "_mask." + fi.suffix();
  m_frame_mask = QImage(maskFileName);

  emit file_name_changed(get_file_name());
}

void SpriteReader::set_device(QIODevice *p_device)
{
  _p_delete_device();
  m_device = p_device;
  m_own_device = false;
  set_state(State::NotLoaded);
  set_error(Error::NoError);
  set_loading_progress(0);
  load();
}

void SpriteReader::load()
{}

void SpriteReader::set_state(State p_state)
{
  if (m_state == p_state)
  {
    return;
  }
  m_state = p_state;
  emit state_changed(m_state);
}

void SpriteReader::set_loading_progress(int p_progress)
{
  p_progress = qBound(0, p_progress, 100);
  if (m_loading_progress == p_progress)
  {
    return;
  }
  m_loading_progress = p_progress;
  emit loading_progress_changed(m_loading_progress);
}

void SpriteReader::set_error(Error p_error)
{
  m_last_error = p_error;
  if (m_last_error != Error::NoError)
  {
    emit error(m_last_error);
  }
}

void SpriteReader::_p_delete_device()
{
  if (m_own_device)
  {
    delete m_device;
    m_device = nullptr;
  }
}
