#ifndef WIDGET_ANIMATOR_H
#define WIDGET_ANIMATOR_H

#include "keyframe_sequence.h"
#include <QObject>

class WidgetAnimator : public KeyframeSequence
{
public:
  WidgetAnimator(QWidget* target);

public:
  void RunSequence(float deltaTime) override;

private:
  QWidget *m_Target = nullptr;

};

#endif // WIDGET_ANIMATOR_H
