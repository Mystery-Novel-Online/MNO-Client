#include "message_metadata.h"
#include "dro/system/effects.h"
#include "dro/system/replay_playback.h"

static MessageMetadata s_CurrentMessage;
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

  void incomingMessage(const QStringList &message)
  {
    s_CurrentMessage.offsetHorizontal = message[CMOffsetH].isEmpty() ? 500 : message[CMOffsetH].toInt();
    s_CurrentMessage.offsetVertical = message[CMOffsetV].toInt();
    s_CurrentMessage.offsetScale = message[CMOffsetS].isEmpty() ? 1000 : message[CMOffsetS].toInt();

    s_CurrentMessage.characterLayers = message[CMSpriteLayers];
    s_CurrentMessage.characterSequence = message[CMAnimSequence];
    s_CurrentMessage.characterPre = message[CMPreAnim];
    s_CurrentMessage.characterFolder = message[CMChrName];
    s_CurrentMessage.characterEmote = message[CMEmote];
    s_CurrentMessage.textContent = message[CMMessage];
    s_CurrentMessage.backgroundPosition = message[CMPosition];
    s_CurrentMessage.sfxName = message[CMSoundName];
    s_CurrentMessage.modifiers.TextColor = (DR::Color)message[CMTextColor].toInt();
    s_CurrentMessage.userShowname = message[CMShowName];
    s_CurrentMessage.characterVideo = message[CMVideoName];
    s_CurrentMessage.modifiers.VisibleDesk = message[CMDeskModifier].toInt();
    s_CurrentMessage.modifiers.Hidden = message[CMHideCharacter].toInt();
    s_CurrentMessage.modifiers.Flipped = message[CMFlipState].toInt();
    s_CurrentMessage.effect = dro::system::effects::effectById(message[CMEffectState].toInt());
    s_CurrentMessage.characterShout = ""; //TO-DO - CMShoutModifier
    s_CurrentMessage.characterId = message[CMChrId].toInt();
    s_CurrentMessage.speakerClient = message[CMClientId].toInt();
    s_CurrentMessage.modifiers.DelaySFX = message[CMSoundDelay].toInt();
    system::replays::recording::messageCharacter();
  }

  void incomingMessage(const ReplayOperation &operation)
  {
    s_CurrentMessage.userShowname = operation.variables["showname"];
    s_CurrentMessage.characterFolder = operation.variables["char"];
    s_CurrentMessage.sfxName = operation.variables["sound"];
    s_CurrentMessage.characterShout = operation.variables["shout"];

    s_CurrentMessage.characterEmote = operation.variables["emote"];
    s_CurrentMessage.characterPre = operation.variables["pre"];
    s_CurrentMessage.characterVideo = operation.variables["video"];
    s_CurrentMessage.backgroundPosition = operation.variables["pos"];
    s_CurrentMessage.modifiers.Hidden = operation.variables["hide"] == "1";
    s_CurrentMessage.modifiers.Flipped = operation.variables["flip"] == "1";
    s_CurrentMessage.effect = dro::system::effects::effectByName(operation.variables["effect"]);
    s_CurrentMessage.textContent = operation.variables["msg"];
  }

  MessageMetadata &recentMessage()
  {
    return s_CurrentMessage;
  }


}
