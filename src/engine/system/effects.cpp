#include "effects.h"
#include "pch.h"
#include "engine/param/json_reader.h"
#include <rolechat/filesystem/RCFile.h>

static QVector<MessageEffect> s_viewportEffects = {};
MessageEffect s_fallbackEffect = MessageEffect("<NONE>");

namespace engine::system::effects
{

  void reload()
  {
    JSONReader effectsReader = JSONReader();
    rolechat::fs::RCFile effectsFile("effects/default/effects.json", false);
    effectsReader.ReadFromFile(effectsFile.findFirst());
    s_viewportEffects = {};

    QJsonArray lEffectsArray = effectsReader.mDocument.array();
    for(QJsonValueRef rEffect : lEffectsArray)
    {
      effectsReader.SetTargetObject(rEffect.toObject());
      MessageEffect lEffectData = MessageEffect(effectsReader.getStringValue("effect_name"));
      lEffectData.loops = effectsReader.getBoolValue("loop");
      lEffectData.ignorePair = effectsReader.getBoolValue("ignore_pair");
      lEffectData.legacyId = effectsReader.getIntValue("id");
      s_viewportEffects.append(lEffectData);
    }
  }

  const MessageEffect &effectByName(QString name)
  {
    for(MessageEffect &effect : s_viewportEffects)
    {
      if(name == effect.name)
        return effect;
    }
    return s_fallbackEffect;
  }

  const MessageEffect &effectById(int id)
  {
    for(MessageEffect &effect : s_viewportEffects)
    {
      if(id == effect.legacyId)
        return effect;
    }
    return s_fallbackEffect;
  }

}
