#include "pair_manager.h"
#include "commondefs.h"
#include <AOApplication.h>
#include <modules/theme/thememanager.h>

PairManager PairManager::s_Instance;

void PairManager::ThemeReload()
{
  m_chatboxOffsetTransforms = {};
  QStringList shownameElements = {"showname", "ao2_chatbox", "message", "chat_arrow"};
  QStringList alignmentElements = {"", "_left", "_right"};

  for(QString elementname : shownameElements)
  {
    for(QString elementalighn : alignmentElements)
    {
      QString fullName = elementname + elementalighn;
      pos_size_type sizing = AOApplication::getInstance()->get_element_dimensions(fullName, COURTROOM_DESIGN_INI);
      if(sizing.height != 0 && sizing.width != 0)
      {
        m_chatboxOffsetTransforms[fullName] = sizing;
      }
    }
  }

}

pos_size_type PairManager::GetElementAlignment(QString name, QString alighment)
{
  QString alignmentName = name + "_" + alighment;

  if(m_chatboxOffsetTransforms.contains(alignmentName)) return m_chatboxOffsetTransforms[alignmentName];
  else if(m_chatboxOffsetTransforms.contains(name)) return m_chatboxOffsetTransforms[name];

  pos_size_type return_value;
  return_value.x = 0;
  return_value.y = 0;
  return_value.width = -1;
  return_value.height = -1;

  return return_value;

}
