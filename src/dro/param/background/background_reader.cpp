#include "background_reader.h"
#include "dro/fs/fs_reading.h"

BackgroundReader::BackgroundReader()
{

}

void BackgroundReader::execLoadBackground(QString t_backgroundName)
{
  const QString l_backgroundJSONPath = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(t_backgroundName) + "/" + "background.json");

  if(FS::Checks::FileExists(l_backgroundJSONPath))
  {
    ReadFromFile(l_backgroundJSONPath);

    QStringList l_positions = {"wit", "def", "pro", "jud", "hld", "hlp"};
    for(QString l_pos : l_positions)
    {
      SetTargetObject("positions");
      
      if(isValueExists(l_pos))
      {
        SetTargetObject(l_pos);
        DRBackgroundPosition l_positionData;
        l_positionData.mBackground = getStringValue("background");
        l_positionData.mForeground = getStringValue("foreground");
        setPosition(l_pos, l_positionData);
      }

      ResetTargetObject();
    }

    SetTargetObject("settings");
    DRBackgroundSettings l_settings;
    l_settings.isDynamic = getBoolValue("dynamic");
    l_settings.mScaleMinimum = getDoubleValue("height_scale_minimum");
    l_settings.mScaleMax = getDoubleValue("height_scale_max");
    l_settings.mPositionMinimum = getDoubleValue("height_position_minimum");
    l_settings.mPositionMaximum = getDoubleValue("height_position_max");
    setSettings(l_settings);

  }
}
