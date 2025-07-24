#ifndef BINARY_ENCODING_H
#define BINARY_ENCODING_H

#include <QVariantList>

namespace dro::encoding
{
class BinaryEncoder
{
public:
  static QString encodeBase64(const QVariantList& values);
  static QVariantList decodeBase64(const QString &base64);
};
}

#endif // BINARY_ENCODING_H
