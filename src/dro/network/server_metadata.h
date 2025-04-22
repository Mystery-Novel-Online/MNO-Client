#ifndef SERVER_METADATA_H
#define SERVER_METADATA_H

class QStringList;
class QString;

namespace ServerMetadata
{
  void SetFeatureList(QStringList features);
  bool FeatureSupported(QString featureName);
}

#endif // SERVER_METADATA_H
