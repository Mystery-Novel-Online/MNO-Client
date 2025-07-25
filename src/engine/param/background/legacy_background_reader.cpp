#include "legacy_background_reader.h"

void LegacyBackgroundReader::loadBackground(const std::string& backgroundPath)
{

  const QString l_positions_ini = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(QString::fromStdString(backgroundPath)) + "/" + "positions.ini");

  QSettings l_settings(l_positions_ini, QSettings::IniFormat);
  l_settings.setIniCodec("UTF-8");
  if (l_settings.status() != QSettings::NoError)
  {
    return;
  }

  const QStringList l_group_list = l_settings.childGroups();
  for (const QString &i_group : l_group_list)
  {
    BackgroundPosition l_positionData;

    const QString l_lower_group = i_group.toLower();
    l_settings.beginGroup(i_group);
    l_positionData.background = l_settings.value("back").toString().toStdString();
    l_positionData.foreground = l_settings.value("front").toString().toStdString();

    //TO-DO: Implement
    const QString l_ambient_sfx = l_settings.value("ambient_sfx").toString();

    assignPosition(l_lower_group.toStdString(), l_positionData);

    l_settings.endGroup();
  }
  return;
}
