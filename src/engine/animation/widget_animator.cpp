#include "widget_animator.h"


WidgetAnimator::WidgetAnimator(QWidget *target) : m_Target(target)
{

}

void WidgetAnimator::RunSequence(float deltaTime)
{
  KeyframeSequence::RunSequence(deltaTime);
  std::unordered_map<std::string, QVariant> evaluatedValues;
  KeyframeSequence::Evaluate(evaluatedValues);
  if (auto it = evaluatedValues.find("position"); it != evaluatedValues.end())
  {
    QVector3D position = it->second.value<QVector3D>();
    m_Target->move(position.x(), position.y());
  }
}
