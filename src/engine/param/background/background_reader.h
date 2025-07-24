#ifndef BACKGROUNDREADER_H
#define BACKGROUNDREADER_H

#include "engine/param/json_reader.h"
#include "rolechat/background/IBackgroundData.h"

using namespace rolechat::background;

class BackgroundReader : public IBackgroundData, public JSONReader
{
public:
  BackgroundReader();
  void loadBackground(const std::string& backgroundPath);
};

#endif // BACKGROUNDREADER_H
