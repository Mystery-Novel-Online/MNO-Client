#ifndef MESSAGE_METADATA_H
#define MESSAGE_METADATA_H

struct MessageModifiers
{
  DialogueType ChatType;
  DR::Color TextColor;
  bool      VisibleDesk = true;
  bool      PreAnimation = false;
  bool      Flipped = false;
  bool      Hidden = true;
  int       ShoutModifier;
  int       EffectState;
  int       DelaySFX;
};

struct PairMetadata
{
  QString characterFolder = "";
  QString characterOutfit = "";
  QString characterEmote = "";
  QString characterSequence = "";
  QString characterLayers = "";

  int offsetScale = 1000;
  int offsetHorizontal = 500;
  int offsetVertical = 0;

  bool isLeader = false;
  bool spriteFlipped = false;
  bool spriteVisible = false;
};

struct MessageMetadata
{
  QString characterPre = "";
  QString characterFolder = "";
  QString characterOutfit = "";
  QString characterEmote = "";
  QString characterSequence = "";
  QString characterLayers = "";
  QString characterVideo = "";
  QString characterShout = "";


  QString messageTags = "";

  QString userShowname = "";
  QString textContent = "";

  QString backgroundPosition = "";
  QString sfxName = "";

  //EffectData
  MessageEffect effect;

  int offsetScale = 1000;
  int offsetHorizontal = 500;
  int offsetVertical = 0;

  int characterId = -1;
  int speakerClient = -1;

  MessageModifiers modifiers;
  PairMetadata pairData;
};

namespace engine::network::metadata::message
{
  int horizontalOffset();
  MessageMetadata& recentMessage();
  void incomingMessage(const QStringList& message);
  void incomingMessage(const ReplayOperation& operation);

  namespace pair
  {
    bool isActive();
    bool isFlipped();
    bool isVisible();
    bool isLeader();
    int verticalOffset();
    int horizontalOffset();
    double scaleOffset();
    int scaleValue();
    const QString &getOutfit();
    const QString &getEmote();
    const QString &getCharacter();
    const QString &getLayers();
    const QString &getAnimation();
    void disable();
  }
  void setPairMetadata(const PairMetadata &data, int selfOffset);
}

#endif // MESSAGE_METADATA_H
