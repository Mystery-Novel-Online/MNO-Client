#include "keyframe_channel.h"

template<typename T>
void KeyframeChannel<T>::AddKeyframe(float time, const T &value, KeyframeCurve curveIn, KeyframeCurve curveOut)
{
  m_Keyframes[time] = {time, value, curveIn, curveOut};
  if(m_Length < time) m_Length = time;
}

template<typename T>
void KeyframeChannel<T>::Cache(int frameRate) const
{

}

template<typename T>
void KeyframeChannel<T>::Evaluate(float time, QVariant &outValue) const
{
  if (m_Keyframes.empty()) return;

  auto next = m_Keyframes.lower_bound(time);

  if (next == m_Keyframes.begin()) {
    outValue = QVariant::fromValue(next->second.value);
    return;
  }

  if (next == m_Keyframes.end()) {
    outValue = QVariant::fromValue(std::prev(next)->second.value);
    return;
  }

  auto prev = std::prev(next);
  const auto& keyframePrev = prev->second;
  const auto& keyframeNext = next->second;

  float timeDistance = keyframeNext.time - keyframePrev.time;
  float t = (time - keyframePrev.time) / timeDistance;
  t = std::clamp(t, 0.0f, 1.0f);

  float tOut = ApplyCurve(t, keyframePrev.curveOut);
  float tIn = ApplyCurve(t, keyframeNext.curveIn);
  float curvedT = (tOut + tIn) * 0.5f;

  T result = Interpolate(keyframePrev.value, keyframeNext.value, curvedT);
  outValue = QVariant::fromValue(result);

}

template<typename T>
const float KeyframeChannel<T>::GetLength() const
{
  return m_Length;
}

template<typename T>
float KeyframeChannel<T>::ApplyCurve(float t, KeyframeCurve curve)
{
  if (curve == CurveLinear) return t;
  if (curve == CurveEase) {
    return t < 0.5f ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
  }
  if (curve == CurveBezier) {
    return t * t * (3.0f - 2.0f * t);
  }
  if (curve == CurveParametric) {
    return (t * t) / (2.0f * (t * t - t) + 1.0f);
  }
  return t;
}

template<typename T>
T KeyframeChannel<T>::Interpolate(const T &a, const T &b, float t)
{
  return a + (b - a) * t;
}

template class KeyframeChannel<float>;
template class KeyframeChannel<QVector3D>;
