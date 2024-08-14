#ifndef SCENETESTINGLABS_H
#define SCENETESTINGLABS_H
#pragma once

#include <QObject>
#include <QWidget>
#include <qlineedit.h>
#include <aobutton.h>
#include "modules/widgets/keyframe_player.h"
#include "modules/scenes/viewport_scene.h"
#include "modules/theme/thememanager.h"
#include "modules/widgets/investigation_display.h"

class SceneTestingLabs : public QWidget
{
  Q_OBJECT
public:
  explicit SceneTestingLabs(QWidget *parent = nullptr);

  void ConstructWidgets();

private slots:
  void OnPlayAnimationClicked();

signals:

private:
  ViewportScene *m_Viewport = nullptr;
  ViewportInvestigationDisplay *m_InvestigationBackground = nullptr;
  KeyframePlayer *m_VPKeyframePlayer = nullptr;
};

#endif // SCENETESTINGLABS_H
