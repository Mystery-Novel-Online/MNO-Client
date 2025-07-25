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

#include "mk2/spriteviewer.h"
#include "pch.h"

using namespace mk2;

SpriteViewer::SpriteViewer(QWidget *parent)
    : QLabel{parent}
    , m_player(new SpritePlayer)
{
  setAlignment(Qt::AlignCenter);

  connect(m_player.get(), SIGNAL(current_frame_changed()), this, SLOT(paint_frame()));
  connect(m_player.get(), SIGNAL(file_name_changed(QString)), this, SIGNAL(file_name_changed(QString)));
  connect(m_player.get(), SIGNAL(reader_changed()), this, SIGNAL(reader_changed()));
  connect(m_player.get(), SIGNAL(started()), this, SIGNAL(started()));
  connect(m_player.get(), SIGNAL(finished()), this, SIGNAL(finished()));
}

SpriteViewer::~SpriteViewer()
{}

rolechat::actor::ActorScalingMode SpriteViewer::get_scaling_mode() const
{
  return m_player->get_scaling_mode();
}

void SpriteViewer::set_scaling_mode(rolechat::actor::ActorScalingMode p_scaling_mode)
{
  m_player->set_scaling_mode(p_scaling_mode);
}

void SpriteViewer::set_play_once(bool p_enabled)
{
  m_player->set_play_once(p_enabled);
}

void SpriteViewer::set_mirror(bool p_enabled)
{
  m_player->set_mirror(p_enabled);
}

QString SpriteViewer::get_file_name() const
{
  return m_player->get_file_name();
}

void SpriteViewer::set_file_name(QString p_file_name)
{
  m_player->set_file_name(p_file_name);
}

QIODevice *SpriteViewer::get_device() const
{
  return m_player->get_device();
}

void SpriteViewer::set_device(QIODevice *p_device)
{
  m_player->set_device(p_device);
}

SpriteReader::ptr SpriteViewer::get_reader() const
{
  return m_player->get_reader();
}

void SpriteViewer::set_reader(SpriteReader::ptr p_reader)
{
  m_player->set_reader(p_reader);
}

SpritePlayer *SpriteViewer::get_player() const
{
  return m_player.get();
}

bool SpriteViewer::is_valid() const
{
  return m_player->is_valid();
}

bool SpriteViewer::is_running() const
{
  return m_player->is_running();
}

void SpriteViewer::start()
{
  m_player->start();
}

void SpriteViewer::restart()
{
  stop();
  start();
}

void SpriteViewer::stop()
{
  m_player->stop();
}

void SpriteViewer::restart(int p_start_frame)
{
  stop();
  start(p_start_frame);
}

void SpriteViewer::start(int p_start_frame)
{
  m_player->start(p_start_frame);
}


int SpriteViewer::get_frame()
{
  return m_player->get_frame();
}

void SpriteViewer::resizeEvent(QResizeEvent *p_event)
{
  QLabel::resizeEvent(p_event);
  m_player->set_size(p_event->size());
  paint_frame();
}

void SpriteViewer::paint_frame()
{
  setPixmap(QPixmap::fromImage(m_player->get_current_frame()));
}
