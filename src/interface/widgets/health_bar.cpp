#include "health_bar.h"


HealthBar::HealthBar(QString name, AOApplication *app, QWidget *parent)
    : AOImageDisplay{parent, app}
    , m_App(app)
    , m_Name(name) {
  if(name == "defense") m_variableIndex = 1;
  if(name == "prosecution") m_variableIndex = 2;
}

void HealthBar::setValue(int value) {
  if(value < 0 || value > 10) {
    return;
  }
  m_currentValue = value;
  set_theme_image(m_Name + "bar" + QString::number(m_currentValue) + ".png");
}

void HealthBar::increaseClicked() {
  int newValue = m_currentValue + 1;
  if(newValue <= 10) {
    m_App->send_server_packet(DRPacket("HP", {QString::number(m_variableIndex), QString::number(newValue)}));
  }
}

void HealthBar::decreaseClicked() {
  int newValue = m_currentValue - 1;
  if(newValue >= 0) {
    m_App->send_server_packet(DRPacket("HP", {QString::number(m_variableIndex), QString::number(newValue)}));
  }
}
