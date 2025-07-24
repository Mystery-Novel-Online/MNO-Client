#ifndef BACKGROUNDREADER_H
#define BACKGROUNDREADER_H

#include "background_data.h"
#include "engine/param/json_reader.h"



class BackgroundReader : public BackgroundData, public JSONReader
{
public:
  BackgroundReader();

  void execLoadBackground(QString t_backgroundName);
};

#endif // BACKGROUNDREADER_H
