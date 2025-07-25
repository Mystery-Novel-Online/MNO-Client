#ifndef LEGACYBACKGROUNDREADER_H
#define LEGACYBACKGROUNDREADER_H
#include "rolechat/background/IBackgroundData.h"

using namespace rolechat::background;
class LegacyBackgroundReader : public IBackgroundData
{
public:
  LegacyBackgroundReader() = default;
  void loadBackground(const std::string& backgroundPath);
};

#endif // LEGACYBACKGROUNDREADER_H
