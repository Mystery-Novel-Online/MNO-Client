#include "animation_reader.h"
#include "dro/animation/keyframe_sequence.h"
#include "dro/animation/keyframe_channel.h"
#include "dro/fs/fs_reading.h"
#include "dro/system/runtime_values.h"
#include "aoapplication.h"

AnimationReader::AnimationReader(const QString &animPath, KeyframeSequence &sequence)
{
  sequence.Cleanup();

  QString animationPath = FS::Paths::FindFile("animations/" + animPath + ".json");
  if(!FS::Checks::FileExists(animationPath)) return;

  ReadFromFile(animationPath);

  loadData(sequence);
}

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
  loadData(sequence);

}

void AnimationReader::loadData(KeyframeSequence &sequence)
{
  static const QMap<QString, KeyframeCurve> curveMap =
  {
    {"linear", CurveLinear},
    {"ease", CurveEase},
    {"bezier", CurveBezier},
    {"parametric", CurveParametric}
  };

  bool animationLoop = getBoolValue("loop");
  QString animationSound = getStringValue("sound");
  sequence.SetSound(animationSound);
  sequence.SetLoop(animationLoop);
  sequence.setViewportTimestamp(getIntValue("viewport_time"));

  QRect targetResolution = getRectangleValue("target_resolution");
  if(targetResolution.width() == 0)
  {
    targetResolution.setWidth(960);
    targetResolution.setHeight(544);
  }

  for(QJsonValueRef overlayData : getArrayValue("layers"))
  {
    SetTargetObject(overlayData.toObject());
    QString overlayName = getStringValue("name");
    QRect overlayRect = getRectangleValue("offset");
    QString overlayRender = getStringValue("order");
    QString assetPath = dro::runtime::values::resolveVariables(getStringValue("asset"));
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

    m_Layers.append({overlayName, "", overlayRender, overlayRect, detach, blendMode, "", assetPath});
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
    else if(channelName == "signals")
    {
      for(QJsonValueRef frameValueRef : getArrayValue("frames"))
      {
        const QJsonObject frameObject = frameValueRef.toObject();
        SetTargetObject(frameObject);

        int frameTime = getIntValue("time");
        QString frameValue = getStringValue("value");

        sequence.AddTimedSignal(frameTime, frameValue.toStdString());
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

        const float xValue = frameValue.x() / float(targetResolution.width()) * 1000.0f;
        const float yValue = frameValue.y() / float(targetResolution.height()) * 1000.0f;

        frameValue = {xValue, yValue, frameValue.z()};



        QString curveInValue = getStringValue("curve_in");
        QString curveOutValue = getStringValue("curve_out");

        if(curveMap.contains(curveInValue.toLower())) frameCurveIn = curveMap[curveInValue];
        if(curveMap.contains(curveOutValue.toLower())) frameCurveOut = curveMap[curveOutValue];


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

        QString curveInValue = getStringValue("curve_in");
        QString curveOutValue = getStringValue("curve_out");

        if(curveMap.contains(curveInValue.toLower())) frameCurveIn = curveMap[curveInValue];
        if(curveMap.contains(curveOutValue.toLower())) frameCurveOut = curveMap[curveOutValue];


        frameChannel->AddKeyframe(frameTime, frameValue, frameCurveIn, frameCurveOut);
      }

      sequence.AddChannel(channelName.toStdString(), std::move(frameChannel));
    }
  }
}
