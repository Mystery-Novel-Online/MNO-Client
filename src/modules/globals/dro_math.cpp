#include "dro_math.h"
#include "qdebug.h"

QVector<bool> HexStringToBits(const QString &t_inputString)
{
  QVector<bool> l_returnData;

  if (t_inputString.size() % 2 != 0)
  {
    qDebug() << "Hex string is not valid length.";
    return l_returnData;
  }

  for (int i = 0; i < t_inputString.size(); i += 2)
  {
    bool l_IsValidHexByte;
    QString l_ByteString = t_inputString.mid(i, 2);
    uint8_t l_Byte = l_ByteString.toUInt(&l_IsValidHexByte, 16);

    if (!l_IsValidHexByte)
    {
      qDebug() << "Invalid hex character in string.";
      return l_returnData;
    }

    for (int j = 0; j < 8; ++j)
    {
      bool bit = (l_Byte >> (7 - j)) & 0x01;
      l_returnData.append(bit);
    }
  }

  return l_returnData;
}

QString BitsToHexString(const QVector<bool> &t_inputVector)
{
  QString l_ReturnString;
  QVector<bool> l_BooleanBits = t_inputVector;

  while (l_BooleanBits.size() % 8 != 0)
  {
    l_BooleanBits.append(false);
  }

  for (int i = 0; i < l_BooleanBits.size(); i += 8) {
    uint8_t byte = 0;
    for (int j = 0; j < 8; ++j) {
      if (l_BooleanBits[i + j]) {
        byte |= (1 << (7 - j));
      }
    }
    l_ReturnString.append(QString("%1").arg(byte, 2, 16, QChar('0')).toUpper());
  }

  return l_ReturnString;
}

int CalcMaximumEntries(int t_dimensionsHeight, int t_entryHeight, int t_spacing)
{
  return t_dimensionsHeight / (t_entryHeight + t_spacing);
};

QString Base64ToString(QString t_value)
{
  QString l_returnData = QString::fromUtf8(QByteArray::fromBase64(t_value.toUtf8()));
  return l_returnData;
}
