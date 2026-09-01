#ifndef EFFECTS_H
#define EFFECTS_H

class QString;
class MessageEffect;

namespace engine::system::effects
{
  void reload();
  const MessageEffect &effectByName(QString name);
  const MessageEffect &effectById(int id);
}
#endif // EFFECTS_H
