#ifndef CUEPARSER_H
#define CUEPARSER_H

class CueParser
{
public:
  static std::vector<CueData> parse(const QString& tagData);
};

#endif // CUEPARSER_H
