#include "packets_timer.h"
#include "courtroom.h"

#include <aoapplication.h>


void PacketTimerResume::HandleIncoming(QStringList t_Contents)
{
  int l_TimerID = t_Contents.at(0).toInt();
  AOApplication::getInstance()->m_courtroom->resume_timer(l_TimerID);
}

void PacketTimerSetTime::HandleIncoming(QStringList t_Contents)
{
  int l_TimerID = t_Contents.at(0).toInt();
  int new_time = t_Contents.at(1).toInt();
  AOApplication::getInstance()->m_courtroom->set_timer_time(l_TimerID, new_time);
}

void PacketTimerLength::HandleIncoming(QStringList t_Contents)
{
  int l_TimerID = t_Contents.at(0).toInt();
  int timestep_length = t_Contents.at(1).toInt();
  AOApplication::getInstance()->m_courtroom->set_timer_timestep(l_TimerID, timestep_length);
}

void PacketTimerFiringInterval::HandleIncoming(QStringList t_Contents)
{
  int l_TimerID = t_Contents.at(0).toInt();
  int l_FiringInterval = t_Contents.at(1).toInt();
  AOApplication::getInstance()->m_courtroom->set_timer_firing(l_TimerID, l_FiringInterval);
}

void PacketTimerPause::HandleIncoming(QStringList t_Contents)
{
  int l_TimerID = t_Contents.at(0).toInt();
  AOApplication::getInstance()->m_courtroom->pause_timer(l_TimerID);
}
