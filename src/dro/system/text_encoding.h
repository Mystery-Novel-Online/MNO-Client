#ifndef TEXT_ENCODING_H
#define TEXT_ENCODING_H


class QString;
class QStringList;
namespace dro::system::encoding::text
{
  QString EncodePacketContents(const QStringList& list);
  QStringList DecodePacketContents(const QString& parameters);


  QString EncodeBase64(const QStringList& list);
  QStringList DecodeBase64(const QString& base64);
}

#endif // TEXT_ENCODING_H
