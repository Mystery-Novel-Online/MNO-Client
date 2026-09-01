#ifndef RUNTIME_LOOP_H
#define RUNTIME_LOOP_H

class RPTypewriter;
class RPViewport;

namespace RuntimeLoop
{
  int uptime();
  void Update();
  void setPause(bool pauseState);
  void setWindowFocus(bool focus);

  void assignTypewriter(RPTypewriter *widget);
  void assignViewport(RPViewport *viewport);
};

#endif // RUNTIME_LOOP_H
