#ifndef TRACKLIST_METADATA_H
#define TRACKLIST_METADATA_H

#include <QStringList>


class TracklistMetadata
{
public:
  static QStringList Parse(QStringList musicList);
  static QStringList GetEverything();
  static QStringList GetCategory(QString categoryName);
  static QStringList GetCategories();

  static void PinTrack(QString musicPath);
};

#endif // TRACKLIST_METADATA_H
