#ifndef RUNTIME_LOOP_H
#define RUNTIME_LOOP_H

class RPTypewriter;

namespace RuntimeLoop
{
  int uptime();
  void Update();

  void assignTypewriter(RPTypewriter *widget);
};

#endif // RUNTIME_LOOP_H
