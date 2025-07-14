#include "scene_widget.h"
#include "rp_widget.h"
#include "rp_line_edit.h"
#include "rp_combo_box.h"

SceneWidget::SceneWidget(ThemeSceneType scene, QWidget *parent) : QMainWindow(parent)
{
  m_currentScene = scene;
}

void SceneWidget::reload()
{
  for (auto [widget, identifier] : m_widgetThemeMap.toStdMap())
  {
    dro::system::theme::applyDimensions(widget, identifier, m_currentScene);

    RPLineEdit  *rpLineEdit   =  dynamic_cast<RPLineEdit*>(widget);
    RPButton    *rpButton     =  dynamic_cast<RPButton*>  (widget);
    RPComboBox  *rpComboBox   =  dynamic_cast<RPComboBox*>(widget);

    if(rpLineEdit != nullptr)
    {
      rpLineEdit->refreshPosition();
      rpLineEdit->refreshCSS();
    }

    if(rpButton != nullptr)
      rpButton->refresh_image();

    if(rpComboBox != nullptr)
      rpComboBox->refreshCSS();

  }
}

RPButton *SceneWidget::createButton(QString name, QString image, std::function<void ()> releasedFunction)
{
  RPButton* newButton = createWidget<RPButton>(name);
  newButton->setName(name);
  newButton->setScene(m_currentScene);
  newButton->set_image(image + ".png");

  QObject::connect(newButton, &QPushButton::pressed, [newButton, image]()
  {
    newButton->set_image(image + "_pressed.png");
  });

  QObject::connect(newButton, &QPushButton::released, [newButton, image, releasedFunction]()
  {
    newButton->set_image(image + ".png");
    if (releasedFunction) {  releasedFunction(); }
  });

  return newButton;
}
