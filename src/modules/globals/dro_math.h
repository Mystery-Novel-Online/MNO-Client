#ifndef DRO_MATH_H
#define DRO_MATH_H

#include <QBitArray>
#include <QVector>
#include <QString>

QVector<bool> HexStringToBits(const QString &t_inputString);
QString BitsToHexString(const QVector<bool> &t_inputVector);

QString Base64ToString(QString t_value);

int CalcMaximumEntries(int t_dimensionsHeight, int t_entryHeight, int t_spacing = 0);

#endif
