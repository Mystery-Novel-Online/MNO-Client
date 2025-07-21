#include "draudioengine.h"

#include "draudioengine_p.h"
#include "pch.h"


static const int UPDATE_TIMER_INTERVAL = 100;

static class DRAudioEngineData
{
public:
  using ptr = DRAudioEnginePrivate::ptr;

  ptr &operator->()
  {
    if (d == nullptr)
    {
      d = ptr(new DRAudioEnginePrivate);

      // create engine shortcut
      d->engine = new DRAudioEngine;

      // create families
      d->family_map.insert(DRAudio::Family::FSystem,
                           DRAudioStreamFamily::ptr(new DRAudioStreamFamily(DRAudio::Family::FSystem)));
      d->family_map.insert(DRAudio::Family::FEffect,
                           DRAudioStreamFamily::ptr(new DRAudioStreamFamily(DRAudio::Family::FEffect)));
      d->family_map.insert(DRAudio::Family::FMusic,
                           DRAudioStreamFamily::ptr(new DRAudioStreamFamily(DRAudio::Family::FMusic)));
      d->family_map.insert(DRAudio::Family::FVideo,
                           DRAudioStreamFamily::ptr(new DRAudioStreamFamily(DRAudio::Family::FVideo)));
      d->family_map.insert(DRAudio::Family::FBlip,
                           DRAudioStreamFamily::ptr(new DRAudioStreamFamily(DRAudio::Family::FBlip)));

      // set family-specific options
      d->family_map.value(DRAudio::Family::FSystem)->set_ignore_suppression(true);

      // create and init event loop
      d->update_timer->setInterval(UPDATE_TIMER_INTERVAL);
      d->update_timer->setSingleShot(false);

      QObject::connect(d->update_timer, SIGNAL(timeout()), d, SLOT(update_current_device()));
      d->update_timer->start();
      d->update_current_device();
    }

    return d;
  }

private:
  ptr d;
} d;

DRAudioEngine::DRAudioEngine(QObject *p_parent)
    : QObject(p_parent)
{
  d->children.append(this);
}

DRAudioEngine::~DRAudioEngine()
{
  d->children.removeAll(this);
}

std::optional<DRAudioDevice> DRAudioEngine::get_current_device()
{
  return d->device;
}

QString DRAudioEngine::get_favorite_device_driver()
{
  return d->favorite_device_driver;
}

std::optional<DRAudioDevice> DRAudioEngine::get_favorite_device()
{
  return d->favorite_device;
}

QVector<DRAudioDevice> DRAudioEngine::get_device_list()
{
  return d->device_list;
}

DRAudioStreamFamily::ptr DRAudioEngine::get_family(DRAudio::Family p_family)
{
  return d->family_map.value(p_family);
}

QList<DRAudioStreamFamily::ptr> DRAudioEngine::get_family_list()
{
  return d->family_map.values();
}

int32_t DRAudioEngine::get_volume()
{
  return d->volume;
}

DRAudio::Options DRAudioEngine::get_options()
{
  return d->options;
}

bool DRAudioEngine::is_option(DRAudio::Option p_option)
{
  return d->options.testFlag(p_option);
}

bool DRAudioEngine::is_suppressed()
{
  return d->options.testFlag(DRAudio::OSuppressed);
}

bool DRAudioEngine::is_suppress_background_audio()
{
  return is_option(DRAudio::OEngineSuppressBackgroundAudio);
}

void DRAudioEngine::set_favorite_device_driver(QString p_driver)
{
  d->favorite_device_driver = p_driver;
  d->update_current_device();
}

void DRAudioEngine::set_volume(int32_t p_volume)
{
  p_volume = std::clamp(p_volume, 0, 100);
  if (d->volume == p_volume)
    return;
  d->volume = p_volume;
  d->update_volume();
  Q_EMIT d->invoke_signal("volume_changed", Q_ARG(int32_t, d->volume));
}

void DRAudioEngine::set_options(DRAudio::Options p_options)
{
  if (d->options == p_options)
    return;
  d->options = p_options;
  d->update_options();
  Q_EMIT d->invoke_signal("options_changed", Q_ARG(DRAudio::Options, d->options));
}

void DRAudioEngine::set_option(DRAudio::Option p_option, bool p_enabled)
{
  DRAudio::Options new_options = d->options;
  new_options.setFlag(p_option, p_enabled);
  set_options(new_options);
}

void DRAudioEngine::set_suppressed(bool p_enabled)
{
  set_option(DRAudio::OSuppressed, p_enabled);
}

void DRAudioEngine::set_suppress_background_audio(bool p_enabled)
{
  set_option(DRAudio::OEngineSuppressBackgroundAudio, p_enabled);
}
