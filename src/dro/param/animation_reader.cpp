#include "animation_reader.h"
#include "dro/animation/keyframe_sequence.h"
#include "dro/animation/keyframe_channel.h"
#include "dro/fs/fs_reading.h"
#include "aoapplication.h"

AnimationReader::AnimationReader(const QString &name, KeyframeSequence &sequence, const QString &character)
{
  sequence.Cleanup();

  QString animationPath = FS::Paths::FindFile("characters/" + character + "/animations/" + name + ".json");

  if(!FS::Checks::FileExists(animationPath))
  {
    animationPath = FS::Paths::FindFile("animations/characters/" + name + ".json");
    if(!FS::Checks::FileExists(animationPath)) return;
  }

  ReadFromFile(animationPath);

  bool animationLoop = getBoolValue("loop");
  QString animationSound = getStringValue("sound");
  sequence.SetSound(animationSound);
  sequence.SetLoop(animationLoop);

  QRect targetResolution = getRectangleValue("target_resolution");

  for(QJsonValueRef overlayData : getArrayValue("layers"))
  {
    SetTargetObject(overlayData.toObject());
    QString overlayName = getStringValue("name");
    QRect overlayRect = getRectangleValue("offset");
    QString overlayRender = getStringValue("order");
    QString blendMode = getStringValue("blend_mode");
    bool detach = getBoolValue("detach");

    if (detach)
    {
      const float xNorm = overlayRect.x() / float(targetResolution.width()) * 1000.0f;
      const float yNorm = overlayRect.y() / float(targetResolution.height()) * 1000.0f;
      const float wNorm = overlayRect.width() / float(targetResolution.width()) * 1000.0f;
      const float hNorm = overlayRect.height() / float(targetResolution.height()) * 1000.0f;

      overlayRect = QRect(int(xNorm), int(yNorm), int(wNorm), int(hNorm));
    }

    m_Layers.append({overlayName, "", overlayRender, overlayRect, detach, blendMode});
  }

  ResetTargetObject();

  for(QJsonValueRef channelValueRef : getArrayValue("channels"))
  {
    const QJsonObject channelObject = channelValueRef.toObject();
    SetTargetObject(channelObject);

    QString channelName = getStringValue("name");
    QString channelType = getStringValue("type");

    if(channelName == "sounds")
    {
      for(QJsonValueRef frameValueRef : getArrayValue("frames"))
      {
        const QJsonObject frameObject = frameValueRef.toObject();
        SetTargetObject(frameObject);

        int frameTime = getIntValue("time");
        QString frameValue = getStringValue("value");

        sequence.AddTimedSound(frameTime, frameValue.toStdString());
      }
    }
    else if(channelType == "vec3")
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
    else if(channelType == "float")
    {
      auto frameChannel = std::make_unique<KeyframeChannel<float>>();

      for(QJsonValueRef frameValueRef : getArrayValue("frames"))
      {
        const QJsonObject frameObject = frameValueRef.toObject();
        SetTargetObject(frameObject);

        int frameTime = getIntValue("time");
        float frameValue = getDoubleValue("value");
        KeyframeCurve frameCurveIn = KeyframeCurve::CurveEase;
        KeyframeCurve frameCurveOut = KeyframeCurve::CurveEase;


        frameChannel->AddKeyframe(frameTime, frameValue, frameCurveIn, frameCurveOut);
      }

      sequence.AddChannel(channelName.toStdString(), std::move(frameChannel));
    }

  }

}
