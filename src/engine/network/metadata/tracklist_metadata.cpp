#include "tracklist_metadata.h"

QMap<QString, QStringList> s_musicList = {};
QStringList s_musicCategories = {};
QStringList s_pinnedMusic = {};

QStringList TracklistMetadata::Parse(QStringList musicList)
{
  s_musicList.clear();
  s_musicCategories.clear();
  QStringList m_ReturnValue = {};

  QString m_CurrentCategory = "-- No Category --";

  bool m_CategoryNext = false;
  for(QString r_MusicTrack : musicList)
  {
    if(r_MusicTrack == "category")
    {
      m_CategoryNext = true;
      continue;
    }

    if(m_CategoryNext)
    {
      m_CurrentCategory = r_MusicTrack;
      m_CategoryNext = false;
      s_musicCategories.append(r_MusicTrack);
      m_ReturnValue.append(r_MusicTrack);
      continue;
    }

    s_musicList[m_CurrentCategory].append(r_MusicTrack);
    m_ReturnValue.append(r_MusicTrack);

  }
  return m_ReturnValue;
}

QStringList TracklistMetadata::GetEverything()
{
  QStringList l_returnValue = {};

  for(QString rMusicCategory : s_musicCategories)
  {
    l_returnValue.append(rMusicCategory);
    for(QString r_MusicTrack : s_musicList[rMusicCategory])
    {
      l_returnValue.append(r_MusicTrack);
    }
  }
  return l_returnValue;
}

QStringList TracklistMetadata::GetCategory(QString categoryName)
{
  if(categoryName == "Pinned") return s_pinnedMusic;
  QStringList l_List = {};
  l_List.append(categoryName);
  if(s_musicList.contains(categoryName))
  {
    l_List.append(s_musicList[categoryName]);
  }
  return l_List;
}

QStringList TracklistMetadata::GetCategories()
{
  return s_musicCategories;
}

void TracklistMetadata::PinTrack(QString musicPath)
{
  if(s_pinnedMusic.contains(musicPath))
  {
    s_pinnedMusic.removeAll(musicPath);
    return;
  }
  s_pinnedMusic.append(musicPath);
}
