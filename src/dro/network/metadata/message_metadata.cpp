#include "message_metadata.h"
#include "modules/theme/thememanager.h"

static ICMessageMetadata s_CurrentMessage;
static bool s_MsgPairActive = false;

namespace dro::network::metadata::message
{

  void setPairMetadata(const PairMetadata &data, int selfOffset)
  {
    s_MsgPairActive = true;
    s_CurrentMessage.offsetHorizontal = selfOffset;
    s_CurrentMessage.pairData = data;
    if(data.offsetScale == 0) s_CurrentMessage.pairData.offsetScale = 1000;
  }

  namespace pair
  {
    bool isActive()
    {
      return s_MsgPairActive;
    }

    void disable()
    {
      s_MsgPairActive = false;
      s_CurrentMessage.offsetHorizontal = 0;
      s_CurrentMessage.pairData.offsetHorizontal = 0;
    }

    bool isFlipped()
    {
      return s_CurrentMessage.pairData.spriteFlipped;
    }

    bool isVisible()
    {
      const QString &emote = s_CurrentMessage.pairData.characterEmote;
      if(emote.isEmpty()) return false;
      if(emote == "../../misc/blank") return false;
      return s_MsgPairActive;
    }

    const QString &getEmote()
    {
      return s_CurrentMessage.pairData.characterEmote;
    }

    const QString &getCharacter()
    {
      return s_CurrentMessage.pairData.characterFolder;
    }

    int verticalOffset()
    {
      return s_CurrentMessage.pairData.offsetVertical;
    }

    int horizontalOffset()
    {
      return s_CurrentMessage.pairData.offsetHorizontal;
    }

    double scaleOffset()
    {
      return (double)s_CurrentMessage.pairData.offsetScale / (double)1000.0f;
    }

    const QString &getLayers()
    {
      return s_CurrentMessage.pairData.characterLayers;
    }

    const QString &getAnimation()
    {
      return s_CurrentMessage.pairData.characterSequence;
    }

  }

  int horizontalOffset()
  {
    return s_CurrentMessage.offsetHorizontal;
  }


}
