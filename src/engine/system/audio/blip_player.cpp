#include "blip_player.h"

const int AOBlipPlayer::BLIP_COUNT = 5;

AOBlipPlayer::AOBlipPlayer(QObject *p_parent)
    : AOObject(p_parent)
{
  m_family = DRAudioEngine::get_family(DRAudio::Family::FBlip);
  m_family->set_capacity(BLIP_COUNT);
}

void AOBlipPlayer::set_blips(QString p_blip)
{
  if (m_name.has_value() && m_name.value() == p_blip)
    return;

  m_name = p_blip;
  m_file = ao_app->get_sfx_noext_path(m_name.value());
}

void AOBlipPlayer::blip_tick()
{
  if (!m_file.has_value())
    return;
  m_family->play_stream(m_file.value());
}
