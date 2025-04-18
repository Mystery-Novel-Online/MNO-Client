#include "legacy_background_reader.h"
#include "aoapplication.h"
#include <QSettings>

LegacyBackgroundReader::LegacyBackgroundReader()
{

}

void LegacyBackgroundReader::execLoadBackground(QString t_backgroundName)
{

  const QString l_positions_ini = AOApplication::getInstance()->find_asset_path(AOApplication::getInstance()->get_background_path(t_backgroundName) + "/" + "positions.ini");

  QSettings l_settings(l_positions_ini, QSettings::IniFormat);
  l_settings.setIniCodec("UTF-8");
  if (l_settings.status() != QSettings::NoError)
  {
    return;
  }

  const QStringList l_group_list = l_settings.childGroups();
  for (const QString &i_group : l_group_list)
  {
    DRBackgroundPosition l_positionData;

    const QString l_lower_group = i_group.toLower();
    l_settings.beginGroup(i_group);
    l_positionData.mBackground = l_settings.value("back").toString();
    l_positionData.mForeground = l_settings.value("front").toString();

    //TO-DO: Implement
    const QString l_ambient_sfx = l_settings.value("ambient_sfx").toString();

    setPosition(l_lower_group, l_positionData);

    l_settings.endGroup();
  }
  return;
}
