#include "CueParser.h"
#include "engine/system/text_encoding.h"
using namespace engine::system::encoding::text;

std::vector<CueData> CueParser::parse(const QString &tagData)
{
  std::vector<CueData> result;
  QStringList tagInformation = DecodeBase64(tagData);

  for(const QString& tagInfo : tagInformation)
  {
    QStringList tagArray = DecodePacketContents(tagInfo);

    if(tagArray.count() != 2)
      continue;

    int position = tagArray[0].toInt();
    QVariantList args = engine::encoding::BinaryEncoder::decodeBase64(tagArray[1]);

    CueData cue;
    cue.timestamp = position;
    cue.type = static_cast<CueType>(args.at(0).toInt());
    cue.arguments = args;

    result.push_back(std::move(cue));
  }

  return result;
}
