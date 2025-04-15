#include "bgm_filter.h"
#include "dro/network/tracklist_metadata.h"


BGMFilter::BGMFilter(QWidget* parent) : QComboBox(parent)
{
}

void BGMFilter::ReloadCategories()
{
  clear();
  addItems({"All", "Pinned"});
  addItems(TracklistMetadata::GetCategories());
}

QStringList BGMFilter::GetMusicList()
{
  QString category = currentText();
  if(category == "All") return TracklistMetadata::GetEverything();
  else return TracklistMetadata::GetCategory(category);
  return {};
}
