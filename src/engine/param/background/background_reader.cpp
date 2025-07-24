#include "background_reader.h"
#include "engine/fs/fs_reading.h"

BackgroundReader::BackgroundReader()
{

}

void BackgroundReader::loadBackground(const std::string& backgroundPath)
{
  const QString l_backgroundJSONPath = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(QString::fromStdString(backgroundPath)) + "/" + "background.json");

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
        BackgroundPosition l_positionData;
        l_positionData.background = getStringValue("background").toStdString();
        l_positionData.foreground = getStringValue("foreground").toStdString();
        assignPosition(l_pos.toStdString(), l_positionData);
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

  }
}
