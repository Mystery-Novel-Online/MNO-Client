#include "drpacket.h"
#include "pch.h"

QString DRPacket::encode(QString p_data)
{
  return p_data.replace("#", "<num>").replace("%", "<percent>").replace("$", "<dollar>").replace("&", "<and>");
}

QString DRPacket::decode(QString p_data)
{
  return p_data.replace("<num>", "#").replace("<percent>", "%").replace("<dollar>", "$").replace("<and>", "&");
}

DRPacket::DRPacket(QString p_header)
    : DRPacket(p_header, {})
{}

DRPacket::DRPacket(QString p_header, QStringList p_content)
{
  m_header = p_header;
  m_content = p_content;
}

const QString &DRPacket::get_header() const
{
  return m_header;
}

const QStringList &DRPacket::get_content() const
{
  return m_content;
}

QString DRPacket::to_string(const bool p_encode) const
{
  QString r_data;
  for (const QString &i_value : qAsConst(m_content))
    r_data += (p_encode ? encode(i_value) : i_value) + "#";
  return m_header + "#" + r_data + "%";
}
