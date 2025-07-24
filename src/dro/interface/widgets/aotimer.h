#ifndef AOTIMER_H
#define AOTIMER_H

class QTimer;

class ManualTimer
{
public:
  QTime get_time()
  {
    return current_time;
  }
  int get_timestep_length()
  {
    return timestep_length;
  }

  void set_time(QTime new_time)
  {
    current_time = new_time;
  }
  void set_timestep_length(int new_timestep_length)
  {
    timestep_length = new_timestep_length;
  }
  void perform_timestep()
  {
    current_time = current_time.addMSecs(timestep_length);
  }

private:
  QTime current_time;
  int timestep_length;
};

class AOTimer : public RPTextEdit
{
  Q_OBJECT

public:
  AOTimer(QString name, QWidget *p_parent);

public slots:
  void update_time();
  void set();
  void resume();
  void pause();
  void redraw();
  void set_time(QTime new_time);
  void set_timestep_length(int new_timestep_length);
  void set_firing_interval(int new_firing_interval);
  void set_concentrate_mode();
  void set_normal_mode();
  void set_fast_forward_mode();

private:
  ManualTimer old_manual_timer; // Pre-update manual timer
  ManualTimer manual_timer;
  QTimer *firing_timer = nullptr;
  QTime start_time = QTime(0, 0);
  // All of this is in miliseconds
  int manual_timer_timestep_length = -12;
  int firing_timer_length = 12;
  int time_spent_in_timestep = 0;
  bool paused;
};

#endif // AOTIMER_H
