#include "message_metadata.h"
#include "modules/theme/thememanager.h"

static ICMessageMetadata s_CurrentMessage;
static bool s_MsgPairActive = false;

namespace dro::network::metadata::message
{
  void setPairMetadata(QString character, QString emote, int offsetSelfHorizontal, int offsetPairHorizontal, bool flipped, int scale, int vertical)
  {
    s_CurrentMessage.offsetHorizontal = offsetSelfHorizontal;

    s_MsgPairActive = true;

    s_CurrentMessage.pairData.characterFolder = character;
    s_CurrentMessage.pairData.characterEmote = emote;

    s_CurrentMessage.pairData.spriteFlipped = flipped;

    if(scale != 0)
      s_CurrentMessage.pairData.offsetScale = scale;
    else
      s_CurrentMessage.pairData.offsetScale = 1000;

    s_CurrentMessage.pairData.offsetVertical = vertical;
    s_CurrentMessage.pairData.offsetHorizontal = offsetPairHorizontal;
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

  }

  int horizontalOffset()
  {
    return s_CurrentMessage.offsetHorizontal;
  }


}
