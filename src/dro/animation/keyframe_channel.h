#ifndef KEYFRAME_CHANNEL_H
#define KEYFRAME_CHANNEL_H

#include <QVariant>

enum KeyframeCurve
{
  CurveLinear,
  CurveEase,
  CurveBezier,
  CurveParametric
};

struct TimedSound {
  float timestamp;
  std::string sound;
};

struct TimedSignal {
  float timestamp;
  std::string signal;
};

class KeyframeChannelTemplate
{
public:
  virtual ~KeyframeChannelTemplate() = default;
  virtual void Cache(int frameRate) const = 0;
  virtual void Evaluate(float time, QVariant& outValue) const = 0;
  virtual const float GetLength() const = 0;
  virtual const std::type_info& GetValueType() const = 0;

};

template<typename T>
class KeyframeChannel : public KeyframeChannelTemplate
{
public:
  struct Keyframe
  {
    float time;
    T value;
    KeyframeCurve curveIn = CurveLinear;
    KeyframeCurve curveOut = CurveLinear;
  };

  void AddKeyframe(float time, const T& value, KeyframeCurve curveIn = CurveLinear, KeyframeCurve curveOut = CurveLinear);
  void Cache(int frameRate) const override;
  void Evaluate(float time, QVariant& outValue) const override;
  const std::type_info& GetValueType() const override { return typeid(T); }
  const float GetLength() const override;

private:
  float m_Length = 0.0f;
  std::map<float, Keyframe> m_Keyframes;
  static float ApplyCurve(float t, KeyframeCurve curve);
  static T Interpolate(const T& a, const T& b, float t);
};

#endif // KEYFRAME_CHANNEL_H
