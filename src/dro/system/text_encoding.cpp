#include "text_encoding.h"
#include "pch.h"

namespace dro::system::encoding::text
{
  QString EncodeBase64(const QStringList& list)
  {
    QString joined = list.join(QLatin1Char('\x1F'));
    QByteArray byteArray = joined.toUtf8();
    QByteArray base64 = byteArray.toBase64();
    return QString::fromUtf8(base64);
  }

  QStringList DecodeBase64(const QString& base64)
  {
    QByteArray byteArray = QByteArray::fromBase64(base64.toUtf8());
    QString decoded = QString::fromUtf8(byteArray);
    return decoded.split(QLatin1Char('\x1F'), Qt::KeepEmptyParts);
  }

  QString EncodePacketContents(const QStringList &list)
  {
    return list.join("#");
  }

  QStringList DecodePacketContents(const QString &parameters)
  {
    return parameters.split("#");
  }

  QString DecodeBase64String(const QString& base64)
  {
    QByteArray byteArray = QByteArray::fromBase64(base64.toUtf8());
    return QString::fromUtf8(byteArray);
  }

}
