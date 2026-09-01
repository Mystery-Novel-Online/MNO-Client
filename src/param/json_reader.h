#ifndef JSONREADER_H
#define JSONREADER_H

class JSONReader
{
public:
  JSONReader() = default;
  void  ReadFromFile(std::string path);
  void  ReadFromFile(QString path);
  void  ReadFromString(QString data);

  void  ResetTargetObject();
  void  SetTargetObject(QJsonObject target);
  void  SetTargetObject(QString string);

  bool        isValueExists(QString value);
  QVector2D   getVector2DValue(QString string);
  QVector3D   getVector3DValue(QString string);
  QVector3D   getVector3DColorValue(QString string);
  QJsonArray  getArrayValue(QString string);
  RPRect  getPositionData(QString string);
  QRect  getRectangleValue(QString string);

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
