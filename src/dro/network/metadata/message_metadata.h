#ifndef MESSAGE_METADATA_H
#define MESSAGE_METADATA_H

#include <QString>

struct PairMetadata
{
  QString characterFolder = "";
  QString characterEmote = "";
  QString characterSequence = "";
  QString characterLayers = "";
  int offsetScale = 1000;
  int offsetHorizontal = 500;
  int offsetVertical = 0;

  bool spriteFlipped = false;
  bool spriteVisible = false;

  // sequenceName = "";
  // characterLayers = "";
};

struct ICMessageMetadata
{
  int offsetScale = 1000;
  int offsetHorizontal = 500;
  int offsetVertical = 0;

  PairMetadata pairData;
};

namespace dro::network::metadata::message
{
  int horizontalOffset();

  namespace pair
  {
    bool isActive();
    bool isFlipped();
    bool isVisible();
    int verticalOffset();
    int horizontalOffset();
    double scaleOffset();
    const QString &getEmote();
    const QString &getCharacter();
    const QString &getLayers();
    const QString &getAnimation();
    void disable();
  }
  void setPairMetadata(const PairMetadata &data, int selfOffset);
}

#endif // MESSAGE_METADATA_H
