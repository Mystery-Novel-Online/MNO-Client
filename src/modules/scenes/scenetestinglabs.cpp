#include "scenetestinglabs.h"
#include <aoimagedisplay.h>


SceneTestingLabs::SceneTestingLabs(QWidget *parent) : QWidget{parent}
{
  ConstructWidgets();
  setWindowTitle("Testing Labs");
  setFocusPolicy(Qt::StrongFocus);
  setAttribute(Qt::WA_DeleteOnClose, true );
}

void SceneTestingLabs::ConstructWidgets()
{


  m_Viewport = new ViewportScene(this);
  ThemeManager::get().AutoAdjustWidgetDimensions(m_Viewport, "viewport", SceneTypeReplays);
  ThemeManager::get().RegisterWidgetGeneric("viewport", m_Viewport);

  m_Viewport->ConstructViewport(SceneTypeReplays);

  m_VPKeyframePlayer = new KeyframePlayer(this);
  m_VPKeyframePlayer->resize(960, 544);

  QLineEdit *m_AnimationName = new QLineEdit(this);
  m_AnimationName->resize(960, 20);
  m_AnimationName->move(0, 544);

  AOButton *m_PlayAnimationButton = new AOButton(this, AOApplication::getInstance());
  m_PlayAnimationButton->resize(300, 20);
  m_PlayAnimationButton->move(0, 564);
  //TaggableLineEdit *m_LineEdit = new TaggableLineEdit(this);
  //m_LineEdit->resize(400, 20);


  connect(m_PlayAnimationButton, &QAbstractButton::clicked, this, &SceneTestingLabs::OnPlayAnimationClicked);


  //Investigation Testing
  m_InvestigationBackground = new ViewportInvestigationDisplay(this, AOApplication::getInstance());
  ThemeManager::get().AutoAdjustWidgetDimensions(m_InvestigationBackground, "viewport", SceneTypeReplays);
  m_InvestigationBackground->SetImageBase("invest.png");
  m_InvestigationBackground->UpdateAlpha("invest_alpha.png", 600, 100);
  //
}

void SceneTestingLabs::OnPlayAnimationClicked()
{
  m_Viewport->ToggleChatbox(false);
  m_Viewport->PlayShoutAnimation("consent");
  //m_InvestigationBackground->set_image("invest.png");
  //m_InvestigationBackground->UpdateAlpha("invest_alpha.png", 200, 100);
}
