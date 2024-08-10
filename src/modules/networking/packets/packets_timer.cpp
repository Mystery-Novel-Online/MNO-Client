#include "packets_timer.h"
#include "courtroom.h"

#include <aoapplication.h>


void PacketTimerResume::HandleIncoming(QStringList t_Contents)
{
  int timer_id = t_Contents.at(0).toInt();
  AOApplication::getInstance()->m_courtroom->resume_timer(timer_id);
}

void PacketTimerSetTime::HandleIncoming(QStringList t_Contents)
{
  int timer_id = t_Contents.at(0).toInt();
  int new_time = t_Contents.at(1).toInt();
  AOApplication::getInstance()->m_courtroom->set_timer_time(timer_id, new_time);
}

void PacketTimerLength::HandleIncoming(QStringList t_Contents)
{
  int timer_id = t_Contents.at(0).toInt();
  int timestep_length = t_Contents.at(1).toInt();
  AOApplication::getInstance()->m_courtroom->set_timer_timestep(timer_id, timestep_length);
}

void PacketTimerFiringInterval::HandleIncoming(QStringList t_Contents)
{
  int timer_id = t_Contents.at(0).toInt();
  int firing_interval = t_Contents.at(1).toInt();
  AOApplication::getInstance()->m_courtroom->set_timer_firing(timer_id, firing_interval);
}

void PacketTimerPause::HandleIncoming(QStringList t_Contents)
{
  int timer_id = t_Contents.at(0).toInt();
  AOApplication::getInstance()->m_courtroom->pause_timer(timer_id);
}
