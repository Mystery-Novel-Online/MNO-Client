#include "server_metadata.h"
#include <QStringList>
#include <QString>

QStringList s_featureList = {};

void ServerMetadata::SetFeatureList(QStringList features)
{
  s_featureList.clear();
  s_featureList = features;
}

bool ServerMetadata::FeatureSupported(QString featureName)
{
  return s_featureList.contains(featureName);
}
