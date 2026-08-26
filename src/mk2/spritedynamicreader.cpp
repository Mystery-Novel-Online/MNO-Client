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

#include "spritedynamicreader.h"

#include "spritecachingreader.h"
#include "spriteseekingreader.h"

#include <QImageReader>
#include <QMutex>
#include <QMutexLocker>
#include <QSemaphore>

#if defined(Q_OS_WINDOWS)
#include <Windows.h>
#include <sysinfoapi.h>

double get_mem_usage_percent(quint64 p_requested_mem_size)
{
  MEMORYSTATUSEX l_mem_status;
  l_mem_status.dwLength = sizeof(MEMORYSTATUSEX);
  if(GlobalMemoryStatusEx(&l_mem_status))
  {
    const quint64 l_used_ram = (l_mem_status.ullTotalPhys - l_mem_status.ullAvailPhys) + p_requested_mem_size;
    if(l_used_ram < l_mem_status.ullTotalPhys)
    {
      const double l_percent = std::abs(((double)l_used_ram / l_mem_status.ullTotalPhys) * 100);
      return l_percent;
    }
    else
    {
      return 100.0;
    }
  }
  else
  {
    return 100.0;
  }
}
#elif defined(Q_OS_LINUX)
#include <sys/sysinfo.h>

double get_mem_usage_percent(quint64 p_needed_ram)
{
  struct sysinfo l_sys_info;
  if(sysinfo(&l_sys_info) != -1)
  {
    const quint64 l_used_ram = (l_sys_info.totalram - l_sys_info.freeram) + p_needed_ram;
    if(l_used_ram < l_sys_info.totalram)
    {
      const double l_percent = std::abs(((double)l_used_ram / l_sys_info.totalram) * 100);
      return l_percent;
    }
    else
    {
      return 100.0;
    }
  }
  else
  {
    return 100.0;
  }
}
#elif defined(Q_OS_MAC)
#include <mach/mach_host.h>
#include <mach/vm_statistics.h>

double get_mem_usage_percent(quint64 p_requested_mem_size)
{
  mach_port_t l_mach_port = mach_host_self();

  vm_size_t l_page_size;
  mach_msg_type_number_t count = sizeof(vm_statistics64_data_t) / sizeof(natural_t);
  if(host_page_size(l_mach_port, &l_page_size) != KERN_SUCCESS)
  {
    return 100;
  }

  vm_statistics64_data_t vm_stats;
  if(host_statistics64(l_mach_port, HOST_VM_INFO,
                        (host_info64_t)&vm_stats, &count) != KERN_SUCCESS)
  {
    return 100;
  }

  const quint64 l_free_ram = (quint64)vm_stats.free_count * l_page_size;
  const quint64 l_total_ram = ((quint64)vm_stats.free_count + vm_stats.active_count + vm_stats.inactive_count + vm_stats.wire_count + vm_stats.zero_fill_count) * l_page_size;

  const quint64 l_used_ram = (l_total_ram - l_free_ram) + p_requested_mem_size;
  if(l_used_ram < l_total_ram)
  {
    const double l_percent = std::abs(((double)l_used_ram / l_total_ram) * 100);
    return l_percent;
  }
  else
  {
    return 100.0;
  }
}
#else
#error Unsupported platform.
#endif

using namespace mk2;

std::atomic_int s_system_memory_threshold = 50;
std::atomic_uint64_t s_total_memory_used = 0;

int SpriteDynamicReader::get_system_memory_threshold()
{
  return s_system_memory_threshold;
}

void SpriteDynamicReader::set_system_memory_threshold(int p_percent)
{
  s_system_memory_threshold = qBound(10, p_percent, 80);
}

SpriteDynamicReader::SpriteDynamicReader(QObject *parent)
    : SpriteReader{parent}
    , m_used_memory{0}
{
  _p_create_reader(true);
}

SpriteDynamicReader::~SpriteDynamicReader()
{
  _p_free_memory();
}

QSize SpriteDynamicReader::get_sprite_size() const
{
  return m_reader->get_sprite_size();
}

int SpriteDynamicReader::get_frame_count() const
{
  return m_reader->get_frame_count();
}

SpriteFrame SpriteDynamicReader::get_frame(int number)
{
  return m_reader->get_frame(number);
}

QVector<SpriteFrame> SpriteDynamicReader::get_frame_list()
{
  return m_reader->get_frame_list();
}

void SpriteDynamicReader::load()
{
  _p_free_memory();
  QImageReader l_image_reader(get_device());
  const QSize l_size = l_image_reader.size();

  bool l_caching = true;
  qint64 l_projected_memory = 0;
  if(l_size.isValid() && l_image_reader.imageCount() > 0)
  {
    l_projected_memory = qint64(((qint64)l_image_reader.size().width() * l_image_reader.size().height()) * l_image_reader.imageCount() * 4);

    if(get_mem_usage_percent(s_total_memory_used + l_projected_memory) > s_system_memory_threshold)
    {
      l_caching = false;
      l_projected_memory = 0;
    }
  }
  m_used_memory = l_projected_memory;
  s_total_memory_used += m_used_memory;

  _p_create_reader(l_caching);
  m_reader->set_device(get_device());
}

void SpriteDynamicReader::_p_create_reader(bool p_caching)
{
  mk2::SpriteReader *l_reader = nullptr;
  if(p_caching)
  {
    l_reader = new SpriteCachingReader;
  }
  else
  {
    l_reader = new SpriteSeekingReader;
  }
  connect(l_reader, SIGNAL(state_changed(mk2::SpriteReader::State)), this, SLOT(set_state(mk2::SpriteReader::State)));
  connect(l_reader, SIGNAL(loading_progress_changed(int)), this, SLOT(set_loading_progress(int)));
  connect(l_reader, SIGNAL(error(mk2::SpriteReader::Error)), this, SLOT(set_error(mk2::SpriteReader::Error)));
  m_reader = mk2::SpriteReader::ptr(l_reader);
}

void SpriteDynamicReader::_p_free_memory()
{
  s_total_memory_used -= m_used_memory;
  m_used_memory = 0;
}
