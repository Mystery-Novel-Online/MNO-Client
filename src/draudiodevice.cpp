#include "draudiodevice.h"
#include "pch.h"

QVector<DRAudioDevice> DRAudioDevice::get_device_list()
{
  QVector<DRAudioDevice> r_device_list;

  BASS_DEVICEINFO l_device_info;
  for (int i = 0; BASS_GetDeviceInfo(i, &l_device_info); ++i)
  {
    DRAudioDevice l_device;
    l_device.m_id = i;
    l_device.m_name = l_device_info.name;
    l_device.m_driver = l_device_info.driver;
    l_device.m_states = State(l_device_info.flags);
    r_device_list.append(l_device);
  }

  return r_device_list;
}

DRAudioDevice::DRAudioDevice()
    : m_id(0)
    , m_name("<unknown>")
{}

DRAudioDevice::~DRAudioDevice()
{}

DWORD DRAudioDevice::get_id() const
{
  return m_id;
}

QString DRAudioDevice::get_name() const
{
  return m_name;
}

QString DRAudioDevice::get_driver() const
{
  return m_driver.isEmpty() ? "NOSOUND" : m_driver;
}

DRAudioDevice::States DRAudioDevice::get_states() const
{
  return m_states;
}

bool DRAudioDevice::is_state(DRAudioDevice::State p_state) const
{
  return m_states.testFlag(p_state);
}

bool DRAudioDevice::is_enabled() const
{
  return is_state(SEnabled);
}

bool DRAudioDevice::is_default() const
{
  return is_state(SDefault);
}

bool DRAudioDevice::is_init() const
{
  return is_state(SInit);
}

bool DRAudioDevice::operator==(const DRAudioDevice &other) const
{
  return m_id == other.m_id && m_name == other.m_name && m_driver == other.m_driver && m_states == other.m_states;
}

bool DRAudioDevice::operator!=(const DRAudioDevice &other) const
{
  return !operator==(other);
}
