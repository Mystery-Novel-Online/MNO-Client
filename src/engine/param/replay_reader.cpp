#include "replay_reader.h"
#include "engine/fs/fs_reading.h"

ReplayReader::ReplayReader(const QString &path, QVector<ReplayOperation> &operations)
{
  operations.clear();
  if(!FS::Checks::FileExists(path)) return;
  ReadFromFile(path);

  QJsonArray scriptOperations = getArrayValue("script");
  for(QJsonValueRef operation : scriptOperations)
  {
    const QJsonObject operationObject = operation.toObject();
    SetTargetObject(operationObject);
    ReplayOperation operationData = {getStringValue("op"), getIntValue("time")};
    QStringList keys = operationObject.keys();

    for(QString operationKey : keys)
      operationData.variables[operationKey] = getStringValue(operationKey);

    operations.append(operationData);
  }
}
