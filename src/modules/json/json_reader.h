#ifndef JSONREADER_H
#define JSONREADER_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QVector2D>


class JSONReader
{
public:
  JSONReader();
  void  ReadFromFile(QString path);
  void  ReadFromString(QString data);

  void  ResetTargetObject();
  void  SetTargetObject(QJsonObject target);
  void  SetTargetObject(QString string);

  bool        isValueExists(QString value);
  QVector2D   getVector2DValue(QString string);
  QJsonArray  getArrayValue(QString string);

  bool        getBoolValue(QString string);
  int         getIntValue(QString string);
  double      getDoubleValue(QString string);

  //Strings
  QString     getStringValue(QString string);
  QStringList getStringArrayValue(QString string);

  QJsonDocument mDocument;
  QJsonObject   mMainObject;
  QJsonObject   mTargetObject;
};

#endif // JSONREADER_H
