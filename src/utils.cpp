#include "utils.h"
#include "pch.h"

using namespace utils;

static QString lookupKey(const QStringList &p_key_list, const QString &p_target_key)
{
  const QString l_lower_target_key = p_target_key.toLower();
  for (const QString &i_key : qAsConst(p_key_list))
    if (i_key.toLower() == l_lower_target_key)
      return i_key;
  return p_target_key;
}

QSettingsKeyFetcher::QSettingsKeyFetcher(QSettings &settings)
    : m_settings(settings)
{}

QString QSettingsKeyFetcher::lookup_group(QString p_name)
{
  return lookupKey(m_settings.childGroups(), p_name);
}

QString QSettingsKeyFetcher::lookup_value(QString p_name)
{
  return lookupKey(m_settings.childKeys(), p_name);
}
