#include "json_reader.h"

void JSONReader::ReadFromFile(std::string path)
{
  ReadFromFile(QString::fromStdString(path));
}

void JSONReader::ReadFromFile(QString path)
{
  QFile json_file(path); json_file.open(QIODevice::ReadOnly | QIODevice::Text);
  QString fileContents = json_file.readAll(); json_file.close();

  static QRegularExpression trailingCommas(R"(,\s*([\]}]))");
  fileContents.replace(trailingCommas, R"(\1)");

  ReadFromString(fileContents);
}

void JSONReader::ReadFromString(QString data)
{
  mDocument = QJsonDocument::fromJson(data.toUtf8());
  mMainObject = mDocument.object();
  mTargetObject = mMainObject;
}

void JSONReader::ResetTargetObject()
{
  mTargetObject = mMainObject;
}

void JSONReader::SetTargetObject(QJsonObject target)
{
  mTargetObject = target;
}

void JSONReader::SetTargetObject(QString string)
{
  SetTargetObject(mTargetObject.value(string).toObject());
}

bool JSONReader::isValueExists(QString value)
{
  return mTargetObject.contains(value);
}

QString JSONReader::getStringValue(QString string)
{
  return mTargetObject.value(string).toString();
}

bool JSONReader::getBoolValue(QString string)
{
  return mTargetObject.value(string).toBool();
}

int JSONReader::getIntValue(QString string)
{
  return mTargetObject.value(string).toInt();
}

double JSONReader::getDoubleValue(QString string)
{
  return mTargetObject.value(string).toDouble();
}

QVector2D JSONReader::getVector2DValue(QString string)
{
  QVector2D return_data = {};
  return_data.setX(mTargetObject.value(string).toObject().value("x").toDouble());
  return_data.setY(mTargetObject.value(string).toObject().value("y").toDouble());
  return return_data;
}

QVector3D JSONReader::getVector3DValue(QString string)
{
  if(!mTargetObject.contains(string)) return QVector3D(1, 1, 1);
  QVector3D return_data = {};
  return_data.setX(mTargetObject.value(string).toObject().value("x").toDouble());
  return_data.setY(mTargetObject.value(string).toObject().value("y").toDouble());
  return_data.setZ(mTargetObject.value(string).toObject().value("z").toDouble());
  return return_data;
}

QVector3D JSONReader::getVector3DColorValue(QString string)
{
  QVector3D return_data = {};
  return_data.setX(mTargetObject.value(string).toObject().value("r").toDouble());
  return_data.setY(mTargetObject.value(string).toObject().value("g").toDouble());
  return_data.setZ(mTargetObject.value(string).toObject().value("b").toDouble());
  return return_data;
}

QJsonArray JSONReader::getArrayValue(QString string)
{
  return mTargetObject.value(string).toArray();
}

RPRect JSONReader::getPositionData(QString string)
{
  RPRect lReturnValue;
  if(mTargetObject.contains(string))
  {
    lReturnValue.x = mTargetObject.value(string).toObject().value("x").toInt();
    lReturnValue.y = mTargetObject.value(string).toObject().value("y").toInt();
    lReturnValue.width = mTargetObject.value(string).toObject().value("width").toInt();
    lReturnValue.height = mTargetObject.value(string).toObject().value("height").toInt();
  }
  return lReturnValue;
}

QRect JSONReader::getRectangleValue(QString string)
{
  QRect l_ReturnValue;
  if(mTargetObject.contains(string))
  {
    const QJsonObject &rectangleObject = mTargetObject.value(string).toObject();
    l_ReturnValue.setX(rectangleObject.value("x").toInt());
    l_ReturnValue.setY(rectangleObject.value("y").toInt());
    l_ReturnValue.setWidth(rectangleObject.value("width").toInt());
    l_ReturnValue.setHeight(rectangleObject.value("height").toInt());
  }
  return l_ReturnValue;
}


QStringList JSONReader::getStringArrayValue(QString string)
{
  QStringList return_data = {};
  QJsonArray data = mTargetObject.value(string).toArray();

  for(QJsonValueRef ref : data)
  {
    return_data.append(ref.toString());
  }
  return return_data;
}
