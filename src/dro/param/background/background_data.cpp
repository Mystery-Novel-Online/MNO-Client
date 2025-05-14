#include "background_data.h"

BackgroundData::BackgroundData()
{

}

void BackgroundData::setPosition(QString t_position, DRBackgroundPosition t_data)
{
  mPositions[t_position] = t_data;
}

QString BackgroundData::getBackgroundFilename(QString t_position)
{
  if(mPositions.contains(t_position))
  {
    if(!mPositions[t_position].mBackground.isEmpty())
    {
      return mPositions[t_position].mBackground;
    }
  }

  return "";
}

QString BackgroundData::getForegroundFilename(QString t_position)
{
  if(mPositions.contains(t_position))
  {
    if(!mPositions[t_position].mForeground.isEmpty())
    {
      return mPositions[t_position].mForeground;
    }
  }

  return "";
}

void BackgroundData::setSettings(DRBackgroundSettings t_settings)
{
  mSettings = t_settings;
}

DRBackgroundSettings BackgroundData::getSettings()
{
  return mSettings;
}
