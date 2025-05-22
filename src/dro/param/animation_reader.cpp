#include "animation_reader.h"
#include "dro/animation/keyframe_sequence.h"
#include "dro/animation/keyframe_channel.h"
#include "dro/fs/fs_reading.h"
#include "aoapplication.h"

AnimationReader::AnimationReader(QString name, KeyframeSequence &sequence)
{
  sequence.Cleanup();
  const QString animationPath = FS::Paths::FindFile("animations/characters/" + name + ".json");

  if(!FS::Checks::FileExists(animationPath)) return;

  ReadFromFile(animationPath);

  bool animationLoop = getBoolValue("loop");

  for(QJsonValueRef channelValueRef : getArrayValue("channels"))
  {
    const QJsonObject channelObject = channelValueRef.toObject();
    SetTargetObject(channelObject);

    QString channelName = getStringValue("name");
    QString channelType = getStringValue("type");

    if(channelType == "vec3")
    {
      auto frameChannel = std::make_unique<KeyframeChannel<QVector3D>>();


      for(QJsonValueRef frameValueRef : getArrayValue("frames"))
      {
        const QJsonObject frameObject = frameValueRef.toObject();
        SetTargetObject(frameObject);

        int frameTime = getIntValue("time");
        QVector3D frameValue = getVector3DValue("value");
        KeyframeCurve frameCurveIn = KeyframeCurve::CurveEase;
        KeyframeCurve frameCurveOut = KeyframeCurve::CurveEase;


        frameChannel->AddKeyframe(frameTime, frameValue, frameCurveIn, frameCurveOut);
      }

      sequence.AddChannel(channelName.toStdString(), std::move(frameChannel));
    }

  }

}
