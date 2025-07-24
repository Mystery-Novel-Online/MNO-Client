#pragma once

class DRAudioDevice
{
public:
  static QVector<DRAudioDevice> get_device_list();

  enum State : DWORD
  {
    SEnabled = BASS_DEVICE_ENABLED,
    SDefault = BASS_DEVICE_DEFAULT,
    SInit = BASS_DEVICE_INIT,
  };
  Q_DECLARE_FLAGS(States, State)

  DRAudioDevice();
  ~DRAudioDevice();

  DWORD get_id() const;
  QString get_name() const;
  QString get_driver() const;
  States get_states() const;
  bool is_state(State state) const;
  bool is_enabled() const;
  bool is_default() const;
  bool is_init() const;

  bool operator==(const DRAudioDevice &other) const;
  bool operator!=(const DRAudioDevice &other) const;

private:
  DWORD m_id;
  QString m_name;
  QString m_driver;
  States m_states;
};
Q_DECLARE_METATYPE(DRAudioDevice)
