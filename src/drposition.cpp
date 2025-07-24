#include "drposition.h"
#include "pch.h"

const QMap<QString, DRPosition> DRPositionMap::LEGACY_POSITION_MAP{
    {
        "wit",
        DRPosition("witnessempty", "stand"),
    },
    {
        "def",
        DRPosition("defenseempty", "defensedesk"),
    },
    {
        "pro",
        DRPosition("prosecutorempty", "prosecutiondesk"),
    },
    {
        "jud",
        DRPosition("judgestand", "judgedesk"),
    },
    {
        "hld",
        DRPosition("helperstand", "helperdesk"),
    },
    {
        "hlp",
        DRPosition("prohelperstand", "prohelperdesk"),
    },
};

DRPosition::DRPosition()
{}

DRPosition::DRPosition(QString p_back, QString p_front)
    : m_back(p_back)
    , m_front(p_front)
{}

DRPosition::DRPosition(QString p_back, QString p_front, QString p_ambient_sfx)
    : m_back(p_back)
    , m_front(p_front)
    , m_ambient_sfx(p_ambient_sfx)
{}

DRPosition::~DRPosition()
{}

QString DRPosition::get_back()
{
  return m_back;
}

QString DRPosition::get_front()
{
  return m_front;
}

QString DRPosition::get_ambient_sfx()
{
  return m_ambient_sfx;
}

void DRPosition::set_back(QString p_back)
{
  m_back = p_back;
}

void DRPosition::set_front(QString p_front)
{
  m_front = p_front;
}

void DRPosition::set_ambient_sfx(QString p_ambient_sfx)
{
  m_ambient_sfx = p_ambient_sfx;
}

DRPositionMap::DRPositionMap()
{}

DRPositionMap::~DRPositionMap()
{}

DRPosition DRPositionMap::get_position(QString p_id)
{
  const QString l_lower_id = p_id.toLower();
  return m_position_map.contains(l_lower_id) ? m_position_map.value(l_lower_id) : LEGACY_POSITION_MAP.value(l_lower_id, LEGACY_POSITION_MAP.value("wit"));
}

void DRPositionMap::set_position(QString p_id, DRPosition p_position)
{
  m_position_map.insert(p_id.toLower(), p_position);
}

bool DRPositionMap::load_file(QString p_filename)
{
  QSettings l_settings(p_filename, QSettings::IniFormat);
  l_settings.setIniCodec("UTF-8");
  if (l_settings.status() != QSettings::NoError)
  {
    qWarning() << "[Position Map]"
               << "warning:"
               << "could not load positions.ini file";
    return false;
  }

  QMap<QString, DRPosition> l_position_map;
  const QStringList l_group_list = l_settings.childGroups();
  for (const QString &i_group : l_group_list)
  {
    const QString l_lower_group = i_group.toLower();
    l_settings.beginGroup(i_group);
    const QString l_back = l_settings.value("back").toString();
    const QString l_front = l_settings.value("front").toString();
    const QString l_ambient_sfx = l_settings.value("ambient_sfx").toString();
    l_position_map.insert(l_lower_group, DRPosition(l_back, l_front, l_ambient_sfx));
    l_settings.endGroup();
  }
  m_position_map = l_position_map;
  return true;
}
