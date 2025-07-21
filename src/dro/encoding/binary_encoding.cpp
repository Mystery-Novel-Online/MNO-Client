#include "binary_encoding.h"

#include <QDataStream>
#include <QDebug>

using namespace dro::encoding;

QString BinaryEncoder::encodeBase64(const QVariantList &values)
{
  QByteArray payload;
  QDataStream payloadStream(&payload, QIODevice::WriteOnly);
  payloadStream.setByteOrder(QDataStream::LittleEndian);

  for (const QVariant& v : values)
  {
    payloadStream << static_cast<qint32>(v.type());

    switch (v.type())
    {
    case QVariant::Int:
      payloadStream << v.toInt();
      break;
    case QVariant::Double:
      payloadStream << v.toDouble();
      break;
    case QVariant::Bool:
      payloadStream << v.toBool();
      break;
    case QVariant::String:
    {
      QString str = v.toString();
      payloadStream << str;
      break;
    }
    default:
      qWarning() << "Unsupported type:" << v.typeName();
      break;
    }
  }

  return QString::fromUtf8(payload.toBase64());
}

QVariantList BinaryEncoder::decodeBase64(const QString &base64)
{
  QVariantList values;

  QByteArray payload = QByteArray::fromBase64(base64.toUtf8());
  QDataStream payloadStream(payload);
  payloadStream.setByteOrder(QDataStream::LittleEndian);

  while (!payloadStream.atEnd())
  {
    qint32 typeInt;
    payloadStream >> typeInt;
    QVariant value;

    switch (typeInt)
    {
    case QVariant::Int:
    {
      int i;
      payloadStream >> i;
      value = i;
      break;
    }
    case QVariant::Double:
    {
      double d;
      payloadStream >> d;
      value = d;
      break;
    }
    case QVariant::Bool:
    {
      bool b;
      payloadStream >> b;
      value = b;
      break;
    }
    case QVariant::String:
    {
      QString str;
      payloadStream >> str;
      value = str;
      break;
    }
    default:
      qWarning() << "Unsupported type in decoding:" << typeInt;
      return QVariantList();
    }

    values << value;
  }

  return values;
}
