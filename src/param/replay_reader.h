#ifndef REPLAY_READER_H
#define REPLAY_READER_H

#include "json_reader.h"


class ReplayReader : public JSONReader
{
public:
  ReplayReader(const QString& path, QVector<ReplayOperation>& operations);
};

#endif // REPLAY_READER_H
